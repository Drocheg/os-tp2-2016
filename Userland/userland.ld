OUTPUT_FORMAT("binary")
ENTRY(init_d)
SECTIONS
{
	.text 0x400000 :
	{
		*(.text*)
		. = ALIGN(0x1000);
		*(.rodata*)
	}
	.data ALIGN(0x1000) :
	{
		*(.data*)
	}
	.bss ALIGN(0x1000) :
	{
		bss = .;
		*(.bss*)
	}
	endOfBinary = .;
}