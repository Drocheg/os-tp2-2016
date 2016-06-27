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



%macro saveState 0

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
	push fs
	push gs
%endmacro


%macro loadState 0

	pop gs
	pop fs
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




%macro irqMacro 1

	;Stack frame
	; push rbp
	; push rsp
	; push rbx
	; push r12
	; push r13
	; push r15

	saveState
	changeToKernel

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


%macro changeToKernel 1

	mov rdi, %1
	call getStack 		; Gets current interruption stack
	mov rbx, rsp		; Saves current process stack into RBX
	mov rsp, rax		; Switches context to kernel mode (switch context into kernel's)
	push rbx 			; Saves process stack in kernel's interrupts stack
%endmacro


int20Receiver:

	saveState				; Saves current process' state
	changeToKernel 0		; Now we are in kernel's conext and user stack pushed into kernel's stack
	pop rdi					; Takes off user stack from kernel's stack
	call timerTickHandler	; Calls handler, which returns the next process' stack
	mov rsp, rax			; Switches to next process' context

	;end of interrupt
	mov al, 20h
	out 20h, al	

	loadState				; Restores process' state
	iretq

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


	