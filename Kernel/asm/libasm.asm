GLOBAL outb
GLOBAL inb
GLOBAL cpuVendor
GLOBAL testSysCall

section .text
	
inb:
    mov rdx, rdi
	in al, dx
	ret

outb:
    mov rdx,rdi
    mov rax,rsi
    out dx,al
    ret

cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

section .bss
