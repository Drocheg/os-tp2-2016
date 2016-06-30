#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <processCommon.h>

#define SYSREAD 1
#define SYSWRITE 2
#define SYSCLEAR 3
#define SCROLL 4
#define SPEAKER 5
#define OPENDATAMODULE 6
#define RUNCODEMODULE 7
#define RAINBOW 8
#define REBOOT 9

/* New Syscalls */
#define MALLOC 10
#define TIME 11
#define CREATE_PROCESS 12
#define SLEEP 13

//Video
#define PAINT_PX 860			//Paint white pixel
#define PAINT_PX_COLOR 861		//Paint color pixel
#define PAINT_REKT 862			//Paint REKTangle
#define FILL_REKT 863			//Fill REKTangle
#define PAINT_IMG 864			//Paint image


struct createProcessParams_s {

	uint64_t parentPid;
	char *name;
	void *entryPoint;
	uint64_t argc;
	char **argv;
};


#endif