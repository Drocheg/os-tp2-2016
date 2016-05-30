EXTERN IRQHandler
EXTERN int80Handler
GLOBAL int20Receiver
GLOBAL int21Receiver
GLOBAL int80Receiver

SECTION .text

%macro irqMacro 1
	;Stack frame
	push rbp
	push rsp
	push rbx
	push r12
	push r13
	push r15

	mov rdi, %1
	call IRQHandler
	
	;Undo stack frame
	pop r15
	pop r13
	pop r12
	pop rbx
	pop rsp
	pop rbp
	iretq
%endmacro

int20Receiver:
	irqMacro 0

int21Receiver:
	irqMacro 1

int80Receiver:
	;Stack frame
	push rbp
	push rsp
	push rbx
	push r12
	push r13
	push r15

	;Parameter registers shouldn't have been modified
	call int80Handler
	
	;Undo stack frame
	pop r15
	pop r13
	pop r12
	pop rbx
	pop rsp
	pop rbp
	iretq