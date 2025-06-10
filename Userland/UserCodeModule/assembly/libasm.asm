GLOBAL sys_call
GLOBAL _div
GLOBAL _ioe
GLOBAL _hlt
GLOBAL _cli
GLOBAL _sti
GLOBAL _cliHlt

sys_call:
    push rbp
    mov rbp, rsp        ; rsp -> [arg6][arg7]

    mov rax, rdi        ; syscall id
    mov rdi, rsi        ; arg1
    mov rsi, rdx        ; arg2
    mov rdx, rcx        ; arg3
    mov r10, r8         ; arg4
    mov r8,  r9         ; arg5
    mov r9,  [rbp+8]      ; arg6
    mov r11, [rbp+16]    ; arg7 → pasaremos en stack manualmente

    int 0x80

    mov rsp, rbp
    pop rbp
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
