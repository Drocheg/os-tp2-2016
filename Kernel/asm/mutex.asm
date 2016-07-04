;EXTERN getKernelStack

GLOBAL tsl

SECTION .text

;uint8_t tsl(uint8_t* lockVariable)
;This took me WAY too long, appreciate the help from http://stackoverflow.com/questions/22424209/tsl-instruction-reference#comment34099683_22424447
tsl:
	LOCK mov al, 1			;LOCK prefix makes these atomic
	LOCK xchg al, [rdi]		;if al == 0, then lock was set
	ret