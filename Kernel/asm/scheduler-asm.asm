GLOBAL set_process_stack
GLOBAL idle_process

%macro push_registers 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi ; RDI -> argc
	push rsi ; RSI -> argv
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro set_inicial_stack 0
    
    ; Set the stack pointer to the top of the stack

    push 0x00 ; Aling
    push 0x00 ; SS
    push 0x00 ; RSP
    push 0x202 ; RFLAGS
    push 0x00 ; CS
    push rcx ; RIP

    push_registers

%endmacro

%macro set_stack_frame 0
    
    lea rax, [rel exit]
    push rax
    mov rbp, rsp

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
    mov [aux], rsp
    mov rsp, rdx

    set_stack_frame

    set_inicial_stack

    ; No se si llamar a scheduler 
    ; int 0x20

    mov rsp, [aux]
    ret

exit:
    mov rax, 0x0C ; syscall exit
    int 0x80 ; syscall

idle_process:
    _hlt
    jmp idle_process

.bss
    aux:
        resb 0x100 ; Stack size
