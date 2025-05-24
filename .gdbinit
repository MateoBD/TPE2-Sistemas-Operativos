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
break main
break _start
break scheduler
break printf

# Mensaje informativo
echo \nGDB configurado correctamente para depuración.\n
echo Kernel cargado en 0x100000\n
echo Módulo de usuario cargado en 0x400000\n
echo Usa 'continue' o 'c' para iniciar la ejecución\n