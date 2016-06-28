#include <interrupts.h>
#include <stddef.h>
#include <kernel-lib.h>
#include <memory.h>
#include <video.h>


// static void *softwareIntStack = NULL;
// static void *softwareIntStackPage = NULL;

// static void *timerIntStack = NULL;
// static void *timerIntStackPage = NULL;

// static void *kbdIntStack = NULL;
// static void *kbdIntStackPage = NULL;

static void * kernelStack = NULL;
static void * kernelStackPage = NULL;


// static uint64_t kernelMode;
// static uint64_t hardwareInterrupt;

uint64_t initializeInterruptStacks() {
	
	// pageManager(POP_PAGE, &softwareIntStackPage);
	// pageManager(POP_PAGE, &timerIntStackPage);
	// pageManager(POP_PAGE, &kbdIntStackPage);
	pageManager(POP_PAGE, &kernelStackPage);
	// if (softwareIntStackPage == NULL 
	// 	|| timerIntStackPage == NULL
	// 	|| kbdIntStackPage == NULL
	// 	|| kernelStackPage == NULL) {
	// 	return -1;
	// }

	if (kernelStackPage == NULL) {
		return -1;
	}
	// softwareIntStack = softwareIntStackPage + PAGE_SIZE - sizeof(uint64_t);
	// timerIntStack = timerIntStackPage + PAGE_SIZE - sizeof(uint64_t);
	// kbdIntStack = kbdIntStackPage + PAGE_SIZE - sizeof(uint64_t);
	kernelStack = kernelStackPage + PAGE_SIZE - sizeof(uint64_t);
	// memset(softwareIntStack, 0, sizeof(uint64_t)); /* NULL terminates the stack */
	// memset(timerIntStack, 0, sizeof(uint64_t)); /* NULL terminates the stack */
	// memset(kbdIntStackPage, 0, sizeof(uint64_t)); /* NULL terminates the stack */
	memset(kernelStack, 0, sizeof(uint64_t)); /* NULL terminates the stack */
	return 0;
}


/* TODO: Make just one function */

// void *getStack(uint64_t stack) {

// 	switch (stack) {
// 		case 0x0: return timerIntStack;
// 		case 0x1: return kbdIntStack;
// 		case 0x80: return softwareIntStack;
// 	}
// 	return NULL;
// }

// void *getSoftwareIntStack() {
// 	return softwareIntStack;
// }

// void *getTimerIntStack() {
// 	return timerIntStack;
// }

// void *getKbdIntStack() {
// 	return kbdIntStack;
// }

void *getKernelStack() {
	return kernelStack;
}





/*
void setHardwareInterrupt() {
	hardwareInterrupt = 1;
}

void clearHardwareInterrupt() {
	hardwareInterrupt = 0;
}

void switchToKernel() {
	kernelMode = 1;
}

void switchToUser() {
	if (kernelMode && hardwareInterrupt) {
		return;
	}
	kernelMode = 0;
}
*/
/*
void *getsoftwareIntStack() {
	if (kernelMode) {
		return getActualStackTop();
	}
	switchToKernel();
	return softwareIntStack;
}*/