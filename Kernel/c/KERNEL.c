#include <stdint.h>
#include <kernel-lib.h>
#include <moduleLoader.h>
#include <video.h>
#include <interrupts.h>
#include <idt.h>
#include <libasm.h>
#include <modules.h>
#include <memory.h>
#include <stddef.h>
#include <time.h>
#include <process.h>
#include <scheduler.h>
#include <fileManager.h>
#include <basicFile.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

/* static const uint64_t PageSize = 0x1000; */
static const uint64_t PageSize = PAGE_SIZE;

void clearBSS(void * bssAddress, uint64_t bssSize);
void * initializeKernelBinary();

static void finishKernel();

int kernel_main(int argc, char *argv[]) {
	
	int32_t ret = 0;
	setGraphicMode();
	ncPrint("Set graphic video mode\n");

	ncClear();
	ncPrint("Welcome to the kernel!\n");

	ncPrint("Setting up IDT...");
	setInterrupt(0x20, (uint64_t)&int20Receiver);
	setInterrupt(0x21, (uint64_t)&int21Receiver);
	setInterrupt(0x80, (uint64_t)&int80Receiver);
	ncPrint("Done.\n");

	/* Sets PIT frequency to 500 Hz (one interruption every 2 ms) */
	ncPrint("Increasing PIT frequency...");
	setPITfrequency(500);	/* Any higher and PC speaker stops responding */
	ncPrint("Done.\n");

	/* Initializes memory management */
	ncPrint("Initializing Memory Management...");
	initializePageStack();
	ncPrint("Done.\n");

	/* Initializes File Manager */
	ncClear();
	ncPrint("Initilzing File Manager...");
	initializeFileManager();
	ncPrint("Done\n");

	ncPrint("Initializing basic files...");
	if(initializeBasicFiles() == -1) {
		ncPrint("failed.\n");
		return 0;
	}
	ncPrint("done.\nCreating basic file...");
	BasicFile f = createBasicFileWithName("test");
	if(f == NULL) {
		ncPrint("failed.\n");
		return 0;
	}
	ncPrint("done.\nWriting 'T'...");
	if(basicFileWriteChar('T', f) == EOF) {
		ncPrint("failed.\n");
		return 0;
	}
	ncPrint("done.\nReading...");
	int c = basicFileReadChar(f);
	if(c == EOF) {
		ncPrint("failed.\n");
		return 0;
	}
	ncPrintChar(c);
	ncPrint("\nDestroying file...");
	if(destroyBasicFile(f) == -1) {
		ncPrint("failed.\n");
		return 0;
	}
	ncPrint("done.\n");
	return 0;


	/* Initializes PCB */
	ncPrint("Initializing PCB...");
	initializePCB();
	ncPrint("Done\n");

	/* Enables interrupts (i.e: PIC Mask, and Interrupts stack) */
	ncPrint("Initializing  context switching interrupts stack...");
	if (initializeInterruptStacks(finishKernel)) {
		ncPrint("Couldn't start the kernel. Aborting\n");
		_cli();
		_halt();
		return -1;
	}
	ncPrint("Done.\n");

	/* Initializes the scheduler */
	ncPrint("Initializing Scheduler...");
	initializeScheduler();
	startScheduler();
	ncPrint("Done.\n");

	ncPrint("Enabling interrupts...");
	//masterPICmask(0x0);	//All interrupts
	masterPICmask(0xFC);	//Keyboard and timer tick
	//masterPICmask(0xFD);	//Keyboard only
	//masterPICmask(0xFE);	//Timer tick only
	//masterPICmask(0xFF);	//No interrupts
	ncPrint("Done.\n");


	/* Add init.d process to the scheduler */
	char *args[] = {"init.d"};
	addProcess(0, "init.d", runCodeModule, 1, args);

	ncPrint("Starting init.d\n");
	_sti();		/* Turns on interruptions (will call scheduler' next process function, to start running processes) */
	_halt();	/* Waits till the timer tick interruption comes */
	
	/* Shouldn't reach this point */
	_cli();
	_halt();
	return 0;

	// ncPrint("Jumping to user space...NOW!\n");
	// ret = runCodeModule();
	
	// ncClear();
	// ncPrint("User space returned with exit code ");
	// ncPrintDec(ret);
	// ncPrint(".\nPreparing to shut down...");

	// ncPrint("\n\n\n\n\n\n\n\n\n\n                    IT IS NOW SAFE TO TURN OFF YOUR COMPUTER");
	// _cli();
	// _halt();
	// return 0;
}

static void finishKernel() {

	ncClear();
	ncPrint("Preparing to shut down...");
	ncPrint("\n\n\n\n\n\n\n\n\n\n                    IT IS NOW SAFE TO TURN OFF YOUR COMPUTER");
	_cli();
	_halt();
}

void clearBSS(void * bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void * getStackBase() {
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary() {
	void * moduleAddresses[] = {
		CODE_MODULE_ADDR,
		DATA_MODULE_ADDR
	};
	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);
	ncPrint("Kernel binary initialized.");
	return getStackBase();
}
