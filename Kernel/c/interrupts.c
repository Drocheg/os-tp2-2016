#include <interrupts.h>
#include <stddef.h>
#include <kernel-lib.h>
#include <memory.h>
#include <video.h>


static void *interruptsStackTop = NULL;
static void *interruptsStackPage = NULL;
static uint64_t kernelMode;
static uint64_t hardwareInterrupt;

uint64_t initializeInterruptsStack() {
	
	pageManager(POP_PAGE, &interruptsStackPage);
	if (interruptsStackPage == NULL) {
		return -1;
	}
	interruptsStackTop = interruptsStackPage + PAGE_SIZE - sizeof(uint64_t);
	memset(interruptsStackTop, 0, sizeof(uint64_t)); /* NULL terminates the interrupts stack */
	kernelMode = 1;
	hardwareInterrupt = 0;
	return 0;
}

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

void *getInterruptsStackTop() {
	if (kernelMode) {
		return getActualStackTop();
	}
	switchToKernel();
	return interruptsStackTop;
}