#include <interrupts.h>
#include <stddef.h>
#include <kernel-lib.h>
#include <memory.h>
#include <video.h>


static void * kernelStack = NULL;
static void * kernelStackPage = NULL;


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
	

	/* NULL terminates the stack */
	memset(kernelStack, 0, sizeof(uint64_t));	

	/* Pushes SS register */
	kernelStack -= sizeof(kernelSS);
	memcpy(kernelStack, &kernelSS, sizeof(kernelSS));

	/* Pushes RSP */
	kernelStack -= sizeof(kernelRSP);
	memcpy(kernelStack, &kernelRSP, sizeof(kernelRSP));

	/* Pushes RFLAGS */
	kernelStack -= sizeof(kernelRFLAGS);
	memcpy(kernelStack, &kernelRFLAGS, sizeof(kernelRFLAGS));

	/* Pushes CS */
	kernelStack -= sizeof(kernelCS);
	memcpy(kernelStack, &kernelCS, sizeof(kernelCS));

	/* Pushes RIP (i.e: process main function direction or entry point) */
	kernelStack -= sizeof(kernelRIP);
	memcpy(kernelStack, &kernelRIP, sizeof(kernelRIP));

	/* Pushes fake registers */
	kernelStack -= 17 * sizeof(uint64_t);
	memset(kernelStack, 0, 17 * sizeof(uint64_t));

	return 0;
}



void *getKernelStack() {
	return kernelStack;
}

