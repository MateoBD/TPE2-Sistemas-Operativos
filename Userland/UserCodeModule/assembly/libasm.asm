GLOBAL sys_call
GLOBAL _div
GLOBAL _ioe
GLOBAL _hlt
GLOBAL _cli
GLOBAL _sti
GLOBAL _cliHlt

sys_call:

    mov rax, rdi
    mov rdi, rsi
    mov rsi, rdx
    mov rdx, rcx
    mov r8, r10

    int 80h

    ret
;test division by zero exeption
_div:
    mov rax, 0
    mov rdx, 0
    div rax
    ret
;test invalid opcode exeption
_ioe:
    ud2
    ret

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

section .rodata
msg db "Hello world", 0
