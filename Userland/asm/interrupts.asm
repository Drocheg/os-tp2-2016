GLOBAL _int20
GLOBAL _int21
GLOBAL _int80

_int20:
	int 0x20

_int21:
	int 0x21

_int80:
	int 0x80
	ret