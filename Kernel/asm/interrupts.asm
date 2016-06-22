EXTERN IRQHandler
EXTERN int80Handler

EXTERN getSoftwareIntStack
EXTERN getTimerIntStack
EXTERN getKbdIntStack

EXTERN getStack

GLOBAL int20Receiver
GLOBAL int21Receiver
GLOBAL int80Receiver
GLOBAL getActualStackTop

SECTION .text

%macro irqMacro 1

	;Stack frame
	push rbp
	push rsp
	push rbx
	push r12
	push r13
	push r15

	;Gets kernel stack
	; call setHardwareInterrupt
	; call getInterruptsStackTop
	mov rdi, %1
	call getStack
	mov r15, rax
	mov rbx, rsp					; Saves current process stack into RBX
	mov rsp, rax					; Switches context to kernel mode
	push rbx						; Saves process stack in kernel's interrupts stack

	;Executes interrupt handler
	mov rdi, %1
	call IRQHandler

	;Restores process context
	pop rbx
	mov rsp, rbx

	;signal pic
	mov al, 20h
	out 20h, al						
	
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


	;Switch to kernel mode
	push rdi 								; Saves first parameter
	push rsi								; Saves second parameter
	push rdx								; Saves third parameter
	push rcx								; Saves fourth parameter
	mov rdi, 0x80
	call getStack							; Gets kernel stack
	pop rcx									; Restores fourth parameter
	pop rdx									; Restores third parameter
	pop rsi 								; Restores second parameter
	pop rdi									; Restores first parameter
	mov rbx, rsp 							; Saves current process stack into RBX
	mov rsp, rax							; Switch context to kernel mode
	push rbx 								; Saves process stack in kernel's interrupts stack
	
	;Executes interrupt handler
	call int80Handler						; Parameter registers shouldn't have been modified

	;Restores process context
	pop rbx
	mov rsp, rbx
	
	;Undo stack frame
	pop r15
	pop r13
	pop r12
	pop rbx
	pop rsp
	pop rbp



	iretq

getActualStackTop:
	mov rax, rsp
	ret


	