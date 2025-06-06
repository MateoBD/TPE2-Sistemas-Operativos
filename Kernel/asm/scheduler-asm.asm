GLOBAL set_process_stack
GLOBAL idle_process
GLOBAL call_int_20

%macro set_inicial_stack 0

    ; Stack frame para same-privilege interrupt (no SS pushed by CPU)
    ; Interrupt frame para iretq (orden: RSP -> RFLAGS -> CS -> RIP):

    push rdx       ; RSP
    push 0x202     ; RFLAGS
    push 0x08      ; CS 
    push rcx       ; RIP
    
    ; Ahora los registros en el orden exacto de push_state:
    push 0x00      ; rax
    push rbx      
    push rcx  
    push rdx   
    push rbp       
    push rdi       ; rdi (argc)
    push rsi       ; rsi (argv)
    push 0x00     
    push 0x00     
    push 0x00      
    push 0x00      
    push 0x00      
    push 0x00      
    push 0x00      
    push 0x00      

%endmacro

;===========================================================
; start:
; - params:
;   - rdi: argc
;   - rsi: argv
;   - rdx: stack
;   - rcx: rip
;===========================================================
set_process_stack:
    mov r8, rsp ; Uso r8 como backup del stack actual

    mov rsp, rdx ; Set the stack pointer to the new stack

    set_inicial_stack

    mov rax, rsp ; Retorno el nuevo rsp

    mov rsp, r8

    ret

idle_process:
    cli
    hlt
    jmp idle_process

call_int_20:
    int 0x20
    ret

