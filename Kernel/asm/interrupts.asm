
GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL saveRegisters
GLOBAL getRegisters
GLOBAL setEscFlag

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler

GLOBAL _int80Handler

GLOBAL _exception0Handler
GLOBAL _exception6Handler

EXTERN irqDispatcher
EXTERN sysCallDispatcher
EXTERN exceptionDispatcher
EXTERN getStackBase
EXTERN showRegisters

SECTION .text

%macro pushState 0
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

%macro popState 0
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

%macro irqHandlerMaster 1
	pushState
    mov byte [esc_flag], 0

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
    cmp byte [esc_flag], 1
    jne .donot_save_registers
    catchRegisters
    .donot_save_registers:
    iretq
%endmacro

%macro catchRegisters 0
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
    mov byte [registers_saved], 1   ; seteo flag de que se guardaron registros [ESTO ESTABA MAL]
%endmacro

%macro exceptionHandler 1
    catchRegisters

	mov rdi, %1 ; pasaje de parametro
	call exceptionDispatcher

    call getStackBase
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

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	    0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

_int80Handler:
	
	mov r9, r8
	mov r8, r10
	mov rcx, rdx
	mov rdx, rsi
	mov rsi, rdi 
	mov rdi, rax
	
	call sysCallDispatcher

	iretq

;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

;Zero Division Exception
_exception6Handler:
	exceptionHandler 6
haltcpu:
	cli
	hlt
	ret

saveRegisters:
    catchRegisters
    ret

getRegisters:
    mov rax, 0       ; ret null
    cmp byte [registers_saved], 1
    jne .not_saved
    mov rax, regs_backup
.not_saved:
    ret

setEscFlag:
    mov byte [esc_flag], 1
    ret

section .rodata
    userland equ 0x400000
    registers_saved db 0

section .bss
    esc_flag resb 1
    regs_backup resq 19