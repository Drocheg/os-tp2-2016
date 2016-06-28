#include <interruptHandlers.h>
#include <video.h>
#include <libasm.h>
#include <syscalls.h>
#include <fileDescriptors.h>
#include <kernel-lib.h>
#include <modules.h>
#include <speaker.h>
#include <keyboard.h>
#include <time.h>
#include <memory.h>
#include <video.h>
#include <process.h>
#include <interrupts.h>




static void timerTick();


int64_t int80Handler(uint64_t syscallID, uint64_t p1, uint64_t p2, uint64_t p3) {
	int64_t result = 0;
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
		case CREATE_PROCESS: {

			struct createProcessParams_s *params = (struct createProcessParams_s *)p1;
			result = addProcess(params->parentPid, params->name, params->entryPoint, params->argc, params->argv);
			*((uint64_t *) p2) = (uint64_t) result;
		}
		break;
		case TIME:
			result = time();
			break;
		default:
			result = -1;
			break;
	}
	return result;
}



uint64_t timerTickHandler(void *stack) {

	tick();
	return nextProcess(stack);
}




void IRQHandler(uint8_t irq) {
	uint64_t key;
	switch(irq) {
		case 0:					//Timer tick
			tick();
			
			break;
		case 1:					//Keyboard
			key = (uint64_t) inb(0x60);	//If we don't read from the keyboard buffer, it doesn't fire interrupts again!
			offerKey((uint8_t) key);
			break;
		default:
			break;
	}
	outb(0x20, 0x20);			//EOI signal
}

// static void timerTick() {  	
//   	if(!noSound())				//NOT an else, both cases might need to be run
//   	{
// 		decreaseTimer();
// 	}
// 	if(noSound())
// 	{
// 		soundOff();
// 	}
// }