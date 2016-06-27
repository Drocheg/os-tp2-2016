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
#define MEMORY 10
#define TIME 11

#define CREATE_PROCESS 12



struct createProcessParams_s {

	uint64_t parentPid;
	char name[MAX_NAME_LENGTH];
	void *entryPoint;
	uint64_t argc;
	char *argv[];
};


#endif