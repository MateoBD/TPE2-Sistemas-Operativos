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
b test_function
b create_process

# b sem_init
# b sem_wait
# b sem_post

# Funciones de teclado
b _irq01Handler
b kd_handler
b interrupts.asm:245

# b shell.c:74

b sys_read
b sys_write

define scheduler_debug
    b _irq00Handler
    b scheduler
    b interrupts.asm:225
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
echo   test_read_syscall    - Testear syscall read con tracing completo\n
echo   test_read_simple     - Test básico de syscall read\n
echo   trace_keyboard_input - Rastrear procesamiento de entrada de teclado\n
echo   test_semaphore_stdin - Testear semáforos de stdin\n
echo   inspect_keyboard_buffer - Inspeccionar estado del buffer de teclado\n
echo \n=== READ SYSCALL TESTING ===\n
echo   1. test_read_syscall (para tracing completo)\n
echo   2. continue\n
echo   3. En el shell, ejecutar algún comando que requiera input\n
echo   4. inspect_keyboard_buffer (para ver el estado del buffer)\n
