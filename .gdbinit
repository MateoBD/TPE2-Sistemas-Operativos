# Establecer preferencias de visualización
set disassembly-flavor intel
set pagination off

# Conectar primero a QEMU
target remote host.docker.internal:1234

# Cargar símbolos del kernel
add-symbol-file /root/Kernel/kernel.elf 0x100000

# Cargar símbolos del módulo de usuario 
add-symbol-file /root/Userland/0000-userCodeModule.elf 0x400000

layout split
layout src
layout regs

# Breakpoints comunes (opcional)
b main
b _start

# Funciones de usuario
# b test_function
# b create_process

# b sem_init
# b sem_wait
# b sem_post

# Funciones de teclado

b test_processes
b test_prio
b test_sync
b my_process_inc
b slowInc
b test_sync.c:16
# b my_block
# b sys_block_process
# b sys_unblock_process

define debug_scheduler
    b _irq00Handler
    b scheduler
    b interrupts.asm:225
end

define debug_int21
    b _irq01Handler
    b kd_handler
    b interrupts.asm:245
end




define inspect_keyboard_buffer
    echo === KEYBOARD BUFFER STATUS ===\n
    printf "chars_at_buffer: %d\n", chars_at_buffer
    printf "char_buffer_index: %d\n", char_buffer_index
    printf "getter_index: %d\n", getter_index
    printf "Buffer contents:\n"
    set $i = 0
    while $i < chars_at_buffer && $i < 10
        set $idx = (getter_index + $i) % 64
        printf "  [%d]: '%c' (0x%02x)\n", $i, char_buffer[$idx], char_buffer[$idx]
        set $i = $i + 1
    end
    if chars_at_buffer > 10
        printf "  ... and %d more characters\n", chars_at_buffer - 10
    end
end

define print_stack
    x/24gx $rsp
end

define check_semaphores
    echo === VERIFICANDO SEMÁFOROS DE STDIN ===\n
    
    if initialized
        set $read_sem_id = global_pipe_manager.pipes[0].read_sem
        set $write_sem_id = global_pipe_manager.pipes[0].write_sem
        
        printf "Read semaphore ID: %d\n", $read_sem_id
        printf "Write semaphore ID: %d\n", $write_sem_id
        
        # Intentar obtener valores de semáforos (si existe la función)
        echo "Para ver valores de semáforos, usar:\n"
        printf "  call get_sem_value(%d)  # read semaphore\n", $read_sem_id
        printf "  call get_sem_value(%d)  # write semaphore\n", $write_sem_id
        
        echo "\nValores esperados:\n"
        echo "  Read semaphore: número de caracteres disponibles para leer\n"
        echo "  Write semaphore: espacios disponibles en buffer (max 511)\n"
    else
        printf "ERROR: Pipe manager no inicializado!\n"
    end
end

define print_semaphore
    if $argc != 1
        echo Usage: print_semaphore <semaphore_id>\n
        echo Example: print_semaphore 0\n
    else
        set $sem_id = $arg0
        
        echo === SEMAPHORE STATE ===\n
        printf "Semaphore ID: %d\n", $sem_id
        
        if $sem_id >= 512
            printf "ERROR: Invalid semaphore ID (max 511)\n"
        else
            # Check if semaphore manager is initialized
            if initialized == 0
                printf "ERROR: Semaphore manager not initialized\n"
            else
                # Get semaphore state
                set $sem_state = sem_queue.sem[$sem_id].state
                
                if $sem_state == 0
                    printf "State: FREE (unused)\n"
                else
                    printf "State: USED\n"
                    printf "Value: %d\n", sem_queue.sem[$sem_id].value
                    printf "Current index: %d\n", sem_queue.sem[$sem_id].current_index
                    printf "Last index: %d\n", sem_queue.sem[$sem_id].last_index
                    
                    # Check for waiting processes
                    printf "Waiting processes:\n"
                    set $has_waiting = 0
                    set $i = 0
                    while $i < 1024
                        set $pid = sem_queue.sem[$sem_id].waiting_processes[$i]
                        if $pid != 1025 && $pid != 0
                            printf "  [%d]: PID %d\n", $i, $pid
                            set $has_waiting = 1
                        end
                        set $i = $i + 1
                    end
                    
                    if $has_waiting == 0
                        printf "  No processes waiting\n"
                    end
                end
            end
        end
        echo \n
    end
end

define print_processes
    echo === PROCESS LIST (Positions 3-6) ===\n
    
    # Check if processes are initialized
    if initialized == 0
        printf "ERROR: Process manager not initialized\n"
    else
        set $pos = 3
        while $pos <= 6
            printf "Position %d:\n", $pos
            
            # Check if position is within bounds
            if $pos >= 1024
                printf "  ERROR: Position out of bounds (max 1023)\n"
            else
                # Get process data
                set $pid = process_table[$pos].pid
                set $state = process_table[$pos].state
                set $priority = process_table[$pos].priority
                
                printf "  PID: %d\n", $pid
                
                # Print process name (first 16 characters to avoid buffer issues)
                printf "  Name: %.16s\n", process_table[$pos].name
                
                # Convert state to string
                if $state == 0
                    printf "  State: READY\n"
                else
                    if $state == 1
                        printf "  State: RUNNING\n"
                    else
                        if $state == 2
                            printf "  State: BLOCKED\n"
                        else
                            if $state == 3
                                printf "  State: TERMINATED\n"
                            else
                                printf "  State: UNKNOWN (%d)\n", $state
                            end
                        end
                    end
                end
            end
            
            printf "\n"
            set $pos = $pos + 1
        end
    end
end
