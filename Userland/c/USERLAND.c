#include <stdint.h>
#include <usrlib.h>
#include <stdlib.h>
#include <terminal.h>

extern char bss;
extern char endOfBinary;
static int bssCheck = 0;

static uint64_t terminal(uint64_t argc, char* argv[]);



int32_t init_d(int argc, char* argv[]) {

	uint64_t terminalPid = 0;
	memset(&bss, 0, &endOfBinary - &bss);	//Clean BSS

	if(bssCheck != 0) {						//Improper BSS setup, abort
		return -1;
	}

	clearScreen();

	char *terminalArgv[] = {"terminal 2.0"};
	terminalPid = createProcess("terminal 2.0", terminal, 1, terminalArgv);
	if (terminalPid < 0) {
		return -1;
	}
	waitpid(terminalPid);
	print("Finished\n");
	exit(0);
}


static uint64_t terminal(uint64_t argc, char* argv[]) {
	terminalMain(argc, argv);
	exit(0);
}