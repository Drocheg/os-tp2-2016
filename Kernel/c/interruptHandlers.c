#include <interruptHandlers.h>
#include <video.h>
#include <libasm.h>
#include <syscalls.h>
#include <fileDescriptors.h>
#include <kernel-lib.h>
#include <modules.h>
#include <speaker.h>
#include <keyboard.h>
#include <memory.h>

static void timerTick();

int64_t int80Handler(uint64_t syscallID, uint64_t p1, uint64_t p2, uint64_t p3) {
	int64_t result;
	_sti();
	//No use of function pointer array because different syscalls take different parameters and have
	//different return values.
	switch(syscallID) {
		case SYSREAD:
			result = sys_read((uint8_t)p1, (char *)p2, p3);
			break;
		case SYSWRITE:
			result = sys_write((uint8_t)p1, (char *)p2, p3);
			break;
		case SYSCLEAR:
			ncClear();
			result = 1;
			break;
		case SCROLL:
			ncScrollLines((uint8_t)p1);
			result = p1;
			break;
		case SPEAKER:
			sys_sound((uint32_t)p1, (uint32_t)p2);
			result = 1;
			break;
		case OPENDATAMODULE:
			result = (int64_t) openDataModule();
			break;
		case RAINBOW:
			ncRAINBOWWWWWW();
			result = 1;
			break;
		case REBOOT:
			outb(0x64, 0xFE);		//http://wiki.osdev.org/%228042%22_PS/2_Controller#CPU_Reset
			result = 1;
			break;
		case MEMORY:
			pageManager((Action)p1, (void **)p2);
			break;
		default:
			result = -1;
			break;
	}
	return result;
}

void IRQHandler(uint8_t irq) {
	uint8_t key;
	switch(irq) {
		case 0:					//Timer tick
			timerTick();
			break;
		case 1:					//Keyboard
			key = inb(0x60);	//If we don't read from the keyboard buffer, it doesn't fire interrupts again!
			offerKey(key);
			break;
		default:
			break;
	}
	outb(0x20, 0x20);			//EOI signal
}

static void timerTick() {  	
  	if(!noSound())				//NOT an else, both cases might need to be run
  	{
		decreaseTimer();
	}
	if(noSound())
	{
		soundOff();
	}
}