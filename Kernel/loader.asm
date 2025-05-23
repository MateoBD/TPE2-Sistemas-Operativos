global loader
extern main
extern initialize_kernel_binary

loader:
	call initialize_kernel_binary	; Set up the kernel binary, and get thet stack address
	mov rsp, rax				; Set up the stack with the returned address
	call main
	call hang

hang:
	cli
	hlt	; halt machine should kernel return
	jmp hang
