GLOBAL set_process_stack
GLOBAL idle_process
GLOBAL call_int_20

%macro set_inicial_stack 0
    
    push 0x00 ; Aling
    push 0x00 ; SS
    push rdx ; RSP
    push 0x202 ; RFLAGS
    push 0x08 ; CS
    push rcx ; RIP
    
	push 0x00
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi ; RDI -> argc
	push rsi ; RSI -> argv
	push 0x00
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

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
    _hlt
    jmp idle_process

call_int_20:
    int 0x20
    ret

