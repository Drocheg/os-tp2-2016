#include <modules.h>
#include <kernel-lib.h>
#include <fileDescriptors.h>

typedef int32_t (*EntryPoint)();
static uint8_t running = 0;

//Jumps to the code module and executes code there
int32_t runCodeModule() {
	if(running) {
		sys_write(STDERR, "Code module already running", 27);
		return -1;
	}
	running = 1;
	int32_t result;
	result = ((EntryPoint)CODE_MODULE_ADDR)();
	running = 0;
	return result;
}

//Gets the starting address of the data module. Does not
//control number of simultaneous accesses to data module.
char * openDataModule() {
	return (char*)DATA_MODULE_ADDR;
}

//Gets the starting address of the data module img. Does not
//control number of simultaneous accesses to data module.
char * openDataImgModule() {
	return (char*)DATA_MODULE_IMG_ADDR;
}