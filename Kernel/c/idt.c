#include <idt.h>


typedef struct 
{
	uint16_t	offset_low;		// offset bits 0..15
	uint16_t	selector;		// a code segment selector in GDT or LDT
	uint8_t		zero1;			// unused, set to 0
	uint8_t		type;			// type and attributes, see below
	uint16_t	offset_middle;	// offset bits 16..31
	uint32_t	offset_high;	// offset bits 32..63
	uint32_t	zero2;			// unused, set to 0
} INTERRUPT_DESCRIPTOR;

#define IDT_SIZE 256


static INTERRUPT_DESCRIPTOR *IDT = (INTERRUPT_DESCRIPTOR *)0;

void setInterrupt(uint8_t index, uint64_t offset) {
	if(index < 0 || index >= IDT_SIZE) return;

	IDT[index].offset_low = offset & 0xFFFF;
	IDT[index].offset_middle = (offset >> 16) & 0xFFFF;
	IDT[index].offset_high = (offset >> 32) & 0xFFFFFFFF;
	
	IDT[index].type = (uint8_t)0x8E;
	IDT[index].selector = (uint16_t)0x8;
	IDT[index].zero1 = 0;
	IDT[index].zero2 = 0;
}