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
b sem_wait
b sem_post

# Funciones de teclado

# b shell.c:74

b sys_read
b sys_write

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

# Agregar a los comandos disponibles
echo \n=== FUNCIONES DE DEBUG DISPONIBLES ===\n
echo   debug_stdin          - Configurar debugging completo para STDIN pipe\n
echo   inspect_stdin_state  - Ver estado completo del pipe STDIN\n
echo   trace_write_stdin    - Rastrear escritura a STDIN\n
echo   trace_read_stdin     - Rastrear lectura de STDIN\n
echo   check_semaphores     - Verificar estado de semáforos\n
echo   step_through_stdin   - Guía paso a paso por todo el proceso\n
echo   inspect_keyboard_buffer - Inspeccionar estado del buffer de teclado\n
echo \n=== DEBUGGING STDIN - SECUENCIA RECOMENDADA ===\n
echo   1. debug_stdin (configurar todos los breakpoints)\n
echo   2. continue (hasta que el sistema esté listo)\n
echo   3. step_through_stdin (guía automática paso a paso)\n
echo   O manualmente:\n
echo   1. debug_stdin\n
echo   2. continue (hasta init_pipe_manager)\n
echo   3. inspect_stdin_state\n
echo   4. continue (presiona una tecla)\n
echo   5. trace_write_stdin\n
echo   6. continue (hasta sys_read)\n
echo   7. trace_read_stdin\n

define debug_stdin
    echo === CONFIGURANDO DEBUG PARA STDIN PIPE ===\n
    
    # Limpiar breakpoints previos
    delete
    
    echo 1. Configurando breakpoints en Keyboard Driver...\n
    # Punto de entrada del handler de teclado
    b kd_handler
    # Cuando se escribe al pipe de STDIN
    b keyboard-driver.c:349
    
    echo 2. Configurando breakpoints en Pipes...\n
    # Inicialización del pipe manager
    b init_pipe_manager
    # Funciones principales de pipes
    b write_pipe
    b read_pipe
    # Puntos críticos en write_pipe
    b pipes.c:195
    b pipes.c:199
    # Puntos críticos en read_pipe  
    b pipes.c:167
    b pipes.c:171
    
    echo 3. Configurando breakpoints en Syscalls...\n
    # Sistema de lectura
    b sys_read
    # Verificar resultado de read_pipe
    b syscalls.c:53
    b syscalls.c:55
    
    echo 4. Configurando breakpoints en Semáforos...\n
    b sem_wait
    b sem_post
    b create_sem
    
    echo === BREAKPOINTS CONFIGURADOS ===\n
    echo Usa los siguientes comandos para debugging:\n
    echo   inspect_stdin_state  - Ver estado completo del pipe STDIN\n
    echo   trace_write_stdin    - Rastrear escritura a STDIN\n
    echo   trace_read_stdin     - Rastrear lectura de STDIN\n
    echo   check_semaphores     - Verificar estado de semáforos\n
    echo \n=== SECUENCIA DE DEBUG RECOMENDADA ===\n
    echo   1. continue (hasta init_pipe_manager)\n
    echo   2. inspect_stdin_state\n
    echo   3. continue (hasta que presiones una tecla)\n
    echo   4. trace_write_stdin\n
    echo   5. continue (hasta sys_read)\n
    echo   6. trace_read_stdin\n
end

define inspect_stdin_state
    echo === ESTADO DEL PIPE STDIN (fd=0) ===\n
    
    # Verificar si está inicializado
    printf "Pipe manager inicializado: %d\n", initialized
    
    if initialized
        # Estado del pipe STDIN
        printf "Estado del pipe: %d (0=FREE, 1=USED)\n", global_pipe_manager.pipes[0].state
        printf "Posición de lectura: %d\n", global_pipe_manager.pipes[0].read_pos
        printf "Posición de escritura: %d\n", global_pipe_manager.pipes[0].write_pos
        
        # IDs de semáforos
        printf "Semáforo de lectura ID: %d\n", global_pipe_manager.pipes[0].read_sem
        printf "Semáforo de escritura ID: %d\n", global_pipe_manager.pipes[0].write_sem
        
        # Contenido del buffer (primeros 10 caracteres)
        printf "Contenido del buffer (primeros 10):\n"
        set $i = 0
        while $i < 10
            printf "  [%d]: 0x%02x ('%c')\n", $i, global_pipe_manager.pipes[0].buffer[$i], global_pipe_manager.pipes[0].buffer[$i]
            set $i = $i + 1
        end
        
        # Total de pipes
        printf "Total de pipes activos: %d\n", global_pipe_manager.pipe_count
    else
        printf "ERROR: Pipe manager no inicializado!\n"
    end
end

define trace_write_stdin
    echo === TRAZANDO ESCRITURA A STDIN ===\n
    
    # Verificar parámetros de write_pipe
    printf "fd: %d\n", fd
    printf "buffer[0]: 0x%02x ('%c')\n", *buffer, *buffer
    printf "count: %d\n", count
    
    # Estado del pipe antes de escribir
    printf "Pipe state: %d\n", global_pipe_manager.pipes[fd].state
    printf "Write pos antes: %d\n", global_pipe_manager.pipes[fd].write_pos
    printf "Read pos antes: %d\n", global_pipe_manager.pipes[fd].read_pos
    
    # Estado de semáforos
    printf "Write semaphore ID: %d\n", global_pipe_manager.pipes[fd].write_sem
    printf "Read semaphore ID: %d\n", global_pipe_manager.pipes[fd].read_sem
    
    echo "Continúa para ver la escritura en acción...\n"
end

define trace_read_stdin
    echo === TRAZANDO LECTURA DE STDIN ===\n
    
    # Verificar parámetros de read_pipe
    printf "fd: %d\n", fd
    printf "count solicitado: %d\n", count
    
    # Estado del pipe antes de leer
    printf "Pipe state: %d\n", global_pipe_manager.pipes[fd].state
    printf "Write pos antes: %d\n", global_pipe_manager.pipes[fd].write_pos
    printf "Read pos antes: %d\n", global_pipe_manager.pipes[fd].read_pos
    
    # Contenido disponible
    set $available = (global_pipe_manager.pipes[fd].write_pos - global_pipe_manager.pipes[fd].read_pos + 512) % 512
    printf "Bytes disponibles para leer: %d\n", $available
    
    if $available > 0
        printf "Próximo carácter a leer: 0x%02x ('%c')\n", global_pipe_manager.pipes[fd].buffer[global_pipe_manager.pipes[fd].read_pos], global_pipe_manager.pipes[fd].buffer[global_pipe_manager.pipes[fd].read_pos]
    else
        printf "Buffer vacío - proceso se bloqueará en sem_wait\n"
    end
    
    echo "Continúa para ver la lectura en acción...\n"
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

define step_through_stdin
    echo === PASO A PASO POR STDIN ===\n
    echo "Este comando te guiará paso a paso por el proceso completo\n"
    echo "1. Inicialización...\n"
    continue
    inspect_stdin_state
    
    echo "\n2. Esperando input de teclado...\n"
    echo "Presiona una tecla en el emulador y luego presiona ENTER aquí"
    read
    continue
    trace_write_stdin
    
    echo "\n3. Continuando con escritura...\n"
    step
    step
    step
    
    echo "\n4. Esperando lectura...\n"
    continue
    trace_read_stdin
    
    echo "\n5. Estado final...\n"
    inspect_stdin_state
end
