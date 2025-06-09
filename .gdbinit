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

b kill_foreground_process


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
    if $argc == 0
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
                                    printf "  State: SLEEPING\n"
                                else
                                    if $state == 4
                                        printf "  State: TERMINATED\n"
                                    else
                                        printf "  State: UNKNOWN (%d)\n", $state
                                    end
                                end
                            end
                        end
                    end
                    
                    printf "  Priority: %d\n", $priority
                    printf "  Stack Base: %p\n", process_table[$pos].stack_base
                    printf "  Stack: %p\n", process_table[$pos].stack
                    printf "  FDs: stdin=%d, stdout=%d\n", process_table[$pos].fds[0], process_table[$pos].fds[1]
                    
                    # Show sleep info if sleeping
                    if $state == 3
                        printf "  Sleep Until: %lu\n", process_table[$pos].sleep_until
                        printf "  Is Sleeping: %d\n", process_table[$pos].is_sleeping
                    end
                end
                
                printf "\n"
                set $pos = $pos + 1
            end
        end
    else
        if $argc == 1
            set $target = $arg0
            echo === PROCESS INFO ===\n
            printf "Searching for process: %d\n", $target
            
            # Check if processes are initialized
            if initialized == 0
                printf "ERROR: Process manager not initialized\n"
            else
                set $found = 0
                set $i = 0
                
                # Search by PID first
                while $i < 1024 && $found == 0
                    if process_table[$i].pid == $target
                        set $found = 1
                        printf "Found process by PID at position %d:\n", $i
                        
                        set $state = process_table[$i].state
                        set $priority = process_table[$i].priority
                        
                        printf "  PID: %d\n", process_table[$i].pid
                        printf "  Name: %.32s\n", process_table[$i].name
                        
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
                                        printf "  State: SLEEPING\n"
                                    else
                                        if $state == 4
                                            printf "  State: TERMINATED\n"
                                        else
                                            printf "  State: UNKNOWN (%d)\n", $state
                                        end
                                    end
                                end
                            end
                        end
                        
                        printf "  Priority: %d\n", $priority
                        printf "  Stack Base: %p\n", process_table[$i].stack_base
                        printf "  Stack: %p\n", process_table[$i].stack
                        printf "  FDs: stdin=%d, stdout=%d\n", process_table[$i].fds[0], process_table[$i].fds[1]
                        printf "  Exit Status: %d\n", process_table[$i].exit_status
                        printf "  Sem Wait ID: %d\n", process_table[$i].sem_wait_id
                        printf "  Is Foreground: %d\n", process_table[$i].is_foreground
                        
                        # Show sleep info if sleeping
                        if $state == 3
                            printf "  Sleep Until: %lu\n", process_table[$i].sleep_until
                            printf "  Is Sleeping: %d\n", process_table[$i].is_sleeping
                        end
                    end
                    set $i = $i + 1
                end
                
                # If not found by PID, try by position
                if $found == 0
                    if $target < 1024
                        printf "Process not found by PID, checking position %d:\n", $target
                        
                        set $state = process_table[$target].state
                        set $priority = process_table[$target].priority
                        
                        printf "  PID: %d\n", process_table[$target].pid
                        printf "  Name: %.32s\n", process_table[$target].name
                        
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
                                        printf "  State: SLEEPING\n"
                                    else
                                        if $state == 4
                                            printf "  State: TERMINATED\n"
                                        else
                                            printf "  State: UNKNOWN (%d)\n", $state
                                        end
                                    end
                                end
                            end
                        end
                        
                        printf "  Priority: %d\n", $priority
                        printf "  Stack Base: %p\n", process_table[$target].stack_base
                        printf "  Stack: %p\n", process_table[$target].stack
                        printf "  FDs: stdin=%d, stdout=%d\n", process_table[$target].fds[0], process_table[$target].fds[1]
                        printf "  Exit Status: %d\n", process_table[$target].exit_status
                        printf "  Sem Wait ID: %d\n", process_table[$target].sem_wait_id
                        printf "  Is Foreground: %d\n", process_table[$target].is_foreground
                        
                        # Show sleep info if sleeping
                        if $state == 3
                            printf "  Sleep Until: %lu\n", process_table[$target].sleep_until
                            printf "  Is Sleeping: %d\n", process_table[$target].is_sleeping
                        end
                    else
                        printf "ERROR: Invalid position %d (max 1023)\n", $target
                    end
                end
            end
        else
            echo Usage: print_processes [pid_or_position]\n
            echo   Without arguments: prints processes at positions 3-6\n
            echo   With argument: prints specific process by PID or position\n
            echo Example: print_processes 5\n
        end
    end
end
