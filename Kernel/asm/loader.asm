global loader
extern kernel_main
extern initializeKernelBinary

loader:
	; Setup huge pages (map 4GB)
	mov rdi, 0x00003000			; Address of PDP table (https://github.com/ReturnInfinity/Pure64/blob/master/Pure64%20Manual.md)
	mov rax, 0x87				; Present + Write + User + Page Size (Table 4-15 of Architectures Software Developer's Manual)
	mov rcx, 4					; 4 page entries
paging:
	stosq						; Store RAX in [RDX]
	add rax, 0x40000000 		; Go to the next 1GB
	dec rcx
	cmp rcx, 0
	jne paging

	call initializeKernelBinary	; Set up the kernel binary, and get thet stack address
	mov rsp, rax				; Set up the stack with the returned address
	call kernel_main
hang:
	hlt							; halt machine should kernel return
	jmp hang
