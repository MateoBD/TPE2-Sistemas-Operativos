# Script específico para debuggear el problema del idle process stack
set disassembly-flavor intel
set pagination off

# Conectar a QEMU
target remote host.docker.internal:1234

# Cargar símbolos
add-symbol-file /root/Kernel/kernel.elf 0x100000
add-symbol-file /root/Userland/0000-userCodeModule.elf 0x400000

# Breakpoints críticos para el problema
b init_processes
b set_process_stack
b idle_process
b _irq00Handler
b scheduler

# Comando personalizado para inspeccionar stack
define inspect_stack
    echo === STACK INSPECTION ===\n
    printf "RSP: 0x%016lx\n", $rsp
    printf "Stack contents (top 20 entries):\n"
    x/20gx $rsp
    echo \n=== REGISTERS ===\n
    info registers
    echo \n
end

# Comando para inspeccionar stack frame del idle process
define inspect_idle_frame
    echo === IDLE PROCESS STACK FRAME ===\n
    printf "Idle PCB RSP: 0x%016lx\n", idle_process_pcb->rsp
    printf "Stack contents at idle RSP:\n"
    x/25gx idle_process_pcb->rsp
    echo \n
end

# Comando para rastrear el problema específico
define trace_idle_setup
    echo === TRACING IDLE PROCESS SETUP ===\n
    # Breakpoint en la inicialización
    break processes.c:92
    commands
        silent
        printf "=== BEFORE set_process_stack ===\n"
        printf "idle_process_pcb: 0x%016lx\n", idle_process_pcb
        printf "stack_base: 0x%016lx\n", idle_process_pcb->stack_base
        printf "STACK_SIZE: %d\n", STACK_SIZE
        printf "idle_process function: 0x%016lx\n", &idle_process
        inspect_stack
        continue
    end
    
    # Breakpoint en set_process_stack
    break set_process_stack
    commands
        silent
        printf "=== IN set_process_stack ===\n"
        printf "rdi (argc): 0x%016lx\n", $rdi
        printf "rsi (argv): 0x%016lx\n", $rsi  
        printf "rdx (stack): 0x%016lx\n", $rdx
        printf "rcx (rip): 0x%016lx\n", $rcx
        inspect_stack
        continue
    end
    
    # Breakpoint después del macro set_inicial_stack
    break scheduler-asm.asm:43
    commands
        silent
        printf "=== AFTER set_inicial_stack macro ===\n"
        inspect_stack
        printf "Final RSP to return: 0x%016lx\n", $rax
        continue
    end
end

# Comando para rastrear el primer scheduler switch
define trace_first_switch
    echo === TRACING FIRST SCHEDULER SWITCH ===\n
    
    # Breakpoint en el timer interrupt
    break _irq00Handler
    commands
        silent
        printf "=== TIMER INTERRUPT TRIGGERED ===\n"
        inspect_stack
        continue
    end
    
    # Breakpoint en scheduler
    break scheduler.c:80
    commands
        silent
        printf "=== IN SCHEDULER ===\n"
        printf "current_process: %d\n", current_process
        if current_process == 0
            printf "About to switch to IDLE PROCESS\n"
            inspect_idle_frame
        end
        continue
    end
    
    # Breakpoint en pop_state (interrupts.asm línea aproximada)
    break interrupts.asm:60
    commands
        silent
        printf "=== BEFORE pop_state ===\n"
        inspect_stack
        continue
    end
    
    # Breakpoint en iretq
    break interrupts.asm:120
    commands
        silent
        printf "=== BEFORE iretq ===\n"
        inspect_stack
        printf "Expected iretq frame (from stack top):\n"
        printf "RIP: 0x%016lx\n", *((uint64_t*)$rsp)
        printf "CS:  0x%016lx\n", *((uint64_t*)($rsp + 8))
        printf "RFLAGS: 0x%016lx\n", *((uint64_t*)($rsp + 16))
        printf "RSP: 0x%016lx\n", *((uint64_t*)($rsp + 24))
        continue
    end
end

echo \n=== DEBUG SCRIPT LOADED ===\n
echo Commands available:\n
echo   inspect_stack      - Show current stack state\n  
echo   inspect_idle_frame - Show idle process stack frame\n
echo   trace_idle_setup   - Trace idle process initialization\n
echo   trace_first_switch - Trace first scheduler switch to idle\n
echo \nTo start debugging:\n
echo   1. trace_idle_setup\n
echo   2. continue\n
echo   3. Wait for timer interrupt\n
echo   4. trace_first_switch\n
