global loader
extern kernel_main
extern initializeKernelBinary

loader:
	call initializeKernelBinary	; Set up the kernel binary, and get thet stack address
	mov rsp, rax				; Set up the stack with the returned address
	call kernel_main
hang:
	hlt							; halt machine should kernel return
	jmp hang
