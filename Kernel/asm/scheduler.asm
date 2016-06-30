GLOBAL yield

SECTION .text

yield:
	int 0x81
	ret