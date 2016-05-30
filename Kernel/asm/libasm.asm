GLOBAL cpuVendor
GLOBAL outb
GLOBAL outw
GLOBAL outd
GLOBAL inb
GLOBAL inw
GLOBAL ind
GLOBAL _cli
GLOBAL _sti
GLOBAL _halt
GLOBAL masterPICmask
GLOBAL slavePICmask

SECTION .text
	
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

; void outb(uint16_t port, uint8_t value)
outb:
	mov rdx, rdi
	mov rax, rsi
	out dx, al
	ret

; void outw(uint16_t port, uint16_t value)
outw:
	mov rdx, rdi
	mov rax, rsi
	out dx, ax
	ret

; void outd(uint16_t port, uint32_t value)
outd:
	mov rdx, rdi
	mov rax, rsi
	out dx, eax
	ret

; uint8_t inb(uint16_t port)
inb:
	mov rdx, rdi
	in al, dx
	ret

; uint16_t inw(uint16_t port)
inw:
	mov rdx, rdi
	in ax, dx
	ret

; uint32_t ind(uint16_t port)
ind:
	mov rdx, rdi
	in eax, dx
	ret

masterPICmask:
	mov rax, rdi
	mov rdi, 0x21
	mov rsi, rax
	call outb
	ret

slavePICmask:
	mov rax, rdi
	mov rdi, 0xA1
	mov rsi, rax
	call outb
	ret	

; void _cli()
_cli:
	cli
	ret

; void _sti()
_sti:
	sti
	ret

;void _halt()
_halt:
    hlt
    ret