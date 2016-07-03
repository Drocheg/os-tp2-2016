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
#include <scheduler.h>
#include <mq.h>
#include <process.h>
#include <file.h>

static void timerTick();


int64_t int80Handler(uint64_t syscallID, uint64_t p1, uint64_t p2, uint64_t p3) {
	int64_t result = 0;
	_sti();

	//No use of function pointer array because different syscalls take different parameters and have
	//different return values.
	switch(syscallID) {
		case SYSREAD:
			//result = sys_read((uint8_t)p1, (char *)p2, p3);
			result = read(p1, (char *) p2, p3);
			break;
		case SYSWRITE:
			// result = sys_write((uint8_t)p1, (char *)p2, p3);
			result = write(p1, (char *) p2, p3);
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
			*((char **) p1) = (char *) openDataModule();
			result = 1;
			break;
		case OPENDATAIMGMODULE:
			*((char **) p1) = (char *) openDataImgModule();
			result = 1;
			break;
		case RAINBOW:
			ncRAINBOWWWWWW();
			result = 1;
			break;
		case REBOOT:
			outb(0x64, 0xFE);		//http://wiki.osdev.org/%228042%22_PS/2_Controller#CPU_Reset
			result = 1;
			break;
		case CREATE_PROCESS: {
				_cli();
				struct createProcessParams_s *params = (struct createProcessParams_s *)p1;
				result = addProcess(params->parentPid, params->name, params->entryPoint, params->argc, params->argv);
				*((uint64_t *) p2) = (uint64_t) result;
				_sti();
			}
			break;
		case TIME:
			*((uint64_t *) p1) = time();
			result = *((uint64_t *) p1);
			break;
		case SLEEP:
			sleep(p1);
			break;
		case WAITPID:
			*((int64_t *) p2) = waitpid(p1);
			break;
		case YIELD:
			yield();
			break;
		case EXIT: //TODO hacer algo con el codigo de error en p1.
			
			terminateProcess();
			//yield(); //TODO yield no esta hecho todavia. Y moverlo a terminateProcess.
			uint64_t aux = 0;
			while (1) {
				if ( (aux % 500000) == 0) {
					ncPrint("Dying... ");
				}
				aux++;
			}
			break;

		case PS:
			printPS();
			break;

		case IPCS:
			printIPCS();
			break;

		case MALLOC: //TODO descomentar esto
			//*((uint64_t *) p1) = malloc(getCurrentPCBIndex(), (int64_t) p2);

			break;

		/* *********
		*	Video
		* *********/
		case PAINT_PX:
			paintPixel(p1, p2);
			break;
		case PAINT_PX_COLOR:
			paintColorPixel(p1, p2, p3);
			break;
		case PAINT_REKT:
			drawRect((REKTangle *) p1);
			break;
		case FILL_REKT:
			fillRect((REKTangle *) p1);
			break;
		case PAINT_IMG:
			paintImg((Image *)p1, p2, p3);
			break;
		/* *********
		*	 MQs
		* *********/
		case MQ_OPEN:
			result = MQopen((char *)p1, (uint32_t)p2);
			*((int64_t *)p3) = result;
			break;
		case MQ_CLOSE:
			result = operateFile(getCurrentPCBIndex(), p1, CLOSE, 0);
			*((int8_t *)p3) = (int8_t)result;
			break;
		case MQ_RECEIVE:
			result = read(p1, (char *)p2, 1);
			*((int8_t *)p3) = (int8_t)result;
			break;
		case MQ_SEND:
			result = write(p1, (char *)p2, 1);
			*((int8_t *)p3) = (int8_t)result;
			break;
		

		/* Others */
		case CHANGE_KBD_MODE:
			changeMode((KeyboardMode) p1);
			break;

		/* *********
		*	Default
		* *********/	
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


void keyboardHandler() {
	attendKeyboard();
}



// void IRQHandler(uint8_t irq) {
// 	uint64_t key;
// 	switch(irq) {
// 		case 0:					//Timer tick
// 			tick();
			
// 			break;
// 		case 1:					//Keyboard
// 			key = (uint64_t) inb(0x60);	//If we don't read from the keyboard buffer, it doesn't fire interrupts again!
// 			offerKey((uint8_t) key);
// 			break;
// 		default:
// 			break;
// 	}
// 	// outb(0x20, 0x20);			//EOI signal
// }

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
