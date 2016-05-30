#include <idt.h>

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