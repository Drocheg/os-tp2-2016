EXTERN IRQHandler
EXTERN timerTickHandler
EXTERN int80Handler

EXTERN getKernelStack
EXTERN nextProcess

GLOBAL int20Receiver
GLOBAL int21Receiver
GLOBAL int80Receiver
GLOBAL int81Receiver



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

%macro saveIRETQHook 0

	mov [rsp - 48], rax
	mov [rsp - 56], rbx
	mov rbx, rsp
	mov rax, 0
	push rax
	push rbx
	mov rax, 0x202
	push rax
	mov rax, 0x8
	push rax
	mov rax, _resumeYield
	push rax
	sub rsp, 16
	pop rbx
	pop rax

%endmacro


%macro changeToKernel 0

	call getKernelStack 	; Gets current interruption stack
	mov rbx, rsp			; Saves current process stack into RBX
	mov rsp, rax			; Switches context to kernel mode (switch context into kernel's)
	push rbx 				; Saves process stack in kernel's interrupts stack
%endmacro

%macro endOfInterrupt 0
	
	mov al, 20h
	out 20h, al
%endmacro



int20Receiver:

	saveState				; Saves current process' state
	changeToKernel			; Now we are in kernel's conext and user stack pushed into kernel's stack
	pop rdi					; Takes off user stack from kernel's stack
	call timerTickHandler	; Calls handler, which returns the next process' stack
	mov rsp, rax			; Switches to next process' context

	endOfInterrupt			; End of interrupt

	loadState				; Restores process' state
	iretq


int21Receiver:
	
	saveState
	;Executes interrupt handler
	mov rdi, 1
	call IRQHandler

	endOfInterrupt
	loadState
	iretq

int80Receiver:
	
	saveState

	;Executes interrupt handler
	call int80Handler						; Parameter registers shouldn't have been modified

	loadState
	iretq



; Same as int20Receiver, but without calling timerTickHandler, but nextProcess
int81Receiver:

	saveState				; Saves current process' state
	changeToKernel			; Now we are in kernel's conext and user stack pushed into kernel's stack
	pop rdi					; Takes off user stack from kernel's stack
	call nextProcess		; Calls handler, which returns the next process' stack
	mov rsp, rax			; Switches to next process' context

	endOfInterrupt			; End of interrupt

	loadState				; Restores process' state
	iretq



	