#include <interrupts.h>
#include <stddef.h>
#include <kernel-lib.h>
#include <stdlib.h>
#include <memory.h>
#include <video.h>


static void *kernelStack = NULL;
static void *kernelStackPage = NULL;



/*
 * Initializes the context switching stack, saving an iretq hook and a state (i.e. general purpose registers)
 * into it.
 * When scheduler is off, or no more processes are in the queue, execution will resume from this saved point.
 * Returns 0 on success, or -1 otherwise
 */
uint64_t initializeInterruptStacks(void *finishKernel) {

	uint64_t kernelSS = 0;
	uint64_t kernelRSP = 0;
	uint64_t kernelRFLAGS = 0x202; 					/* WYRM value (sets IF and PF) */
	uint64_t kernelCS = 0x008; 						/* WYRM value (TODO: Ask Rodrigo) */
	uint64_t kernelRIP = (uint64_t) finishKernel;
	
	
	pageManager(POP_PAGE, &kernelStackPage);
	if (kernelStackPage == NULL) {
		return -1;
	}
	kernelStack = kernelStackPage + PAGE_SIZE - sizeof(uint64_t);
	return 0;
}



void *getKernelStack() {
	return kernelStack;
}
