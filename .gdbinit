# Establecer preferencias de visualización
set disassembly-flavor intel
set pagination off

# Conectar primero a QEMU
target remote host.docker.internal:1234

# Cargar símbolos del kernel
add-symbol-file /root/Kernel/kernel.elf 0x100000

# Cargar símbolos del módulo de usuario 
add-symbol-file /root/Userland/0000-userCodeModule.elf 0x400000

# Breakpoints comunes (opcional)
b main
b _start

# scheduler
b _irq00Handler
b scheduler
b interrupts.asm:225

# Funciones de usuario
b test_function
b create_process

# b sem_init
# b sem_wait

# b init_sem_manager
# b create_semaphore
# b semaphore_wait
# b semaphore_post

# b set_process_stack
b idle_process

# b shell.c:56
# b scheduler.c:80
# b kill_process
# b call_int_20

# Configuración de vista múltiple
layout split     # Muestra código fuente y ensamblador
layout src      # Añade los registros
layout regs

# Mensaje informativo
echo \nGDB configurado correctamente para depuración.\n
echo Kernel cargado en 0x100000\n
echo Módulo de usuario cargado en 0x400000\n
echo Usa 'continue' o 'c' para iniciar la ejecución\n
