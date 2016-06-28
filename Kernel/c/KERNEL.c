#include <stdint.h>
#include <kernel-lib.h>
#include <moduleLoader.h>
#include <video.h>
#include <interrupts.h>
#include <idt.h>
#include <libasm.h>
#include <modules.h>
#include <memory.h>
#include <time.h>
#include <stddef.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

void clearBSS(void * bssAddress, uint64_t bssSize);
void * initializeKernelBinary();

int kernel_main(int argc, char *argv[]) {	
	setGraphicMode();
	ncPrint("Set graphic video mode\n");

	ncClear();
	ncPrint("Welcome to the kernel!\n");

	ncPrint("Setting up IDT...");
	setInterrupt(0x20, (uint64_t)&int20Receiver);
	setInterrupt(0x21, (uint64_t)&int21Receiver);
	setInterrupt(0x80, (uint64_t)&int80Receiver);
	ncPrint("Done.\n");

	ncPrint("Enabling interrupts...");
	//masterPICmask(0x0);	//All interrupts
	masterPICmask(0xFC);	//Keyboard and timer tick
	//masterPICmask(0xFD);	//Keyboard only
	//masterPICmask(0xFE);	//Timer tick only
	//masterPICmask(0xFF);	//No interrupts
	_sti();
	ncPrint("Done.\n");

	ncPrint("Increasing PIT frequency...");
	setPITfrequency(500);	//Any higher and PC speaker stops responding
	ncPrint("Done.\n");

	ncPrint("Initializing Memory Management...");
	initializePageStack();
	ncPrint("Done.\n");

	// ncClear();
	// uint8_t *imgData = (uint8_t *)0x500000;
	// uint64_t *imgData2 = (uint64_t *)0x500000;
	// uint64_t width = imgData2[0],
	// 			height = imgData2[1];
	// imgData += 5670;
	// for(int y = 0; y < height; y++) {
	// 	for(int x = 0; x < width; x++) {
	// 		uint8_t red = *(imgData++);
	// 		uint8_t green = *(imgData++);
	// 		uint8_t blue = *(imgData++);
	// 		paintPixelRGB(x, y, red, green, blue);
	// 	}
	// }
	// return 0;


	ncPrint("Jumping to user space...NOW!\n");
	int32_t ret;
	ret = runCodeModule();
	
	ncClear();
	ncPrint("User space returned with exit code ");
	ncPrintDec(ret);
	ncPrint(".\nPreparing to shut down...");

	ncPrint("\n\n\n\n\n\n\n\n\n\n                    IT IS NOW SAFE TO TURN OFF YOUR COMPUTER");
	_cli();
	_halt();
	return 0;
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