;EXTERN getKernelStack

GLOBAL tsl

SECTION .text

;uint8_t tsl(uint8_t* lockVariable);
tsl:
	;LOCK prefix makes these atomic
	LOCK mov rax, 0
	LOCK mov al, [rdi]
	LOCK mov bl, 0
	LOCK mov bh, 1
	LOCK cmpxchg bh, bl
	LOCK mov [rdi], al
	ret

;Pseudocode:
;	rax = 0
;	if([rdi] == 1) {	;Lock already set, do nothing (al will remain at 0)
;		ZF = 1;
;		bh = bl;
;	}
;	else {				;Lock is free, set al = 1
;		ZF = 0;
;		al = 1;
;	}
;	[rdi] = al			;Put our result back at [rdi]


;	if(accumulator == Destination) {
;		ZF = 1;
;		Destination = Source;
;	}
;	else {
;		ZF = 0;
;		accumulator = Destination;
;	}