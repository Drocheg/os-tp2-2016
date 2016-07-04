;EXTERN getKernelStack

GLOBAL tsl

SECTION .text

;uint8_t tsl(uint8_t* lockVariable)
;This took me WAY too long, appreciate the help from http://stackoverflow.com/questions/22424209/tsl-instruction-reference#comment34099683_22424447
tsl:
	mov al, 1
	xchg al, [rdi]		;if al == 0, then lock was set
	ret

;XCGH is atomic - from the Intel manual, Vol. 2A page 3-493:
;The XCHG instruction always asserts the LOCK# signal regardless of the presence or absence of the LOCK prefix.
