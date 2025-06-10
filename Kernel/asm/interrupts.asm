
GLOBAL _cli
GLOBAL _sti
GLOBAL pic_master_mask
GLOBAL pic_slave_mask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL save_registers
GLOBAL get_registers
GLOBAL set_esc_flag

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler

GLOBAL _int80Handler

EXTERN irq_dispatcher
EXTERN syscall_dispatcher
EXTERN exception_dispatcher
EXTERN get_stack_base
EXTERN show_registers
EXTERN scheduler

SECTION .text

%macro push_state 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro pop_state 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro push_state_no_rax 0
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro pop_state_no_rax 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
%endmacro

%macro signal_eoi 0
    push rax
    mov al, 20h
    out 20h, al
    pop rax
%endmacro

%macro irq_handler_master 1
	push_state

	mov rdi, %1 ; pasaje de parametro
	call irq_dispatcher

	signal_eoi

	pop_state
    iretq
%endmacro

%macro catch_registers 0
    mov qword [regs_backup], rax
    mov rax, regs_backup
    add rax, 8
    mov qword [rax], rbx
    add rax, 8
    mov qword [rax], rcx
    add rax, 8
    mov qword [rax], rdx
    add rax, 8
    mov qword [rax], rdi
    add rax, 8
    mov qword [rax], rsi
    add rax, 8
    mov qword [rax], rsp
    add rax, 8
    mov qword [rax], rbp
    add rax, 8
    mov qword [rax], r8
    add rax, 8
    mov qword [rax], r9
    add rax, 8
    mov qword [rax], r10
    add rax, 8
    mov qword [rax], r11
    add rax, 8
    mov qword [rax], r12
    add rax, 8
    mov qword [rax], r13
    add rax, 8
    mov qword [rax], r14
    add rax, 8
    mov qword [rax], r15
    add rax, 8
    mov qword rdi, [rsp]      ; rip
    mov qword [rax], rdi
    add rax, 8
    mov qword rdi, [rsp+8]    ; cs
    mov qword [rax], rdi
    add rax, 8
    mov qword rdi, [rsp+16]   ; rflags
    mov qword [rax], rdi
    mov byte [registers_saved], 1  
%endmacro

%macro exception_handler 1
    catch_registers

	mov rdi, %1 ; pasaje de parametro
	call exception_dispatcher

    call get_stack_base
    mov [rsp+8*3], rax
    mov rax, userland
    mov [rsp], rax
    iretq
%endmacro

;espera a que ocurra una interrupcion 
_hlt:
	sti
	hlt
	ret
;desabilita interrupciones de Hardware
_cli:
	cli
	ret

;habilita interrupciones de Hardware
_sti:
	sti
	ret

pic_master_mask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

pic_slave_mask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	    0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
	push_state

    mov rdi, 0
	call irq_dispatcher

    mov rdi, rsp
    call scheduler
    mov rsp, rax

	signal_eoi

	pop_state

    iretq

;Keyboard
_irq01Handler:
    push_state
    mov byte [esc_flag], 0

	mov rdi, 1 ; pasaje de parametro
	call irq_dispatcher

	signal_eoi

	pop_state
    cmp byte [esc_flag], 1
    jne .donot_save_registers
    catch_registers
    .donot_save_registers:
    iretq

;Forzar un cambio de contexto (sin llamar a timer_handler)
_irq02Handler:
	push_state

    mov rdi, rsp
    call scheduler
    mov rsp, rax

	signal_eoi

	pop_state

    iretq

;Serial Port 2 and 4
_irq03Handler:
	irq_handler_master 3

;Serial Port 1 and 3
_irq04Handler:
	irq_handler_master 4

;USB
_irq05Handler:
	irq_handler_master 5

_int80Handler:
    push rbp
    mov rbp, rsp

    push_state_no_rax

    push r9
    push r11
    mov r9, r8
    mov r8, r10
    mov rcx, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, rax

    call syscall_dispatcher

    pop r11
    pop r9

    signal_eoi

    pop_state_no_rax


    mov rsp, rbp
    pop rbp
    iretq
    
haltcpu:
	cli
	hlt
	ret

save_registers:
    catch_registers
    ret

get_registers:
    mov rax, 0       ; ret null
    cmp byte [registers_saved], 1
    jne .not_saved
    mov rax, regs_backup
.not_saved:
    ret

set_esc_flag:
    mov byte [esc_flag], 1
    ret

section .rodata
    userland equ 0x400000
    registers_saved db 0

section .bss
    esc_flag resb 1
    regs_backup resq 19