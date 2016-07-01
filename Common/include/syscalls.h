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
#define OPENDATAIMGMODULE 13
#define EXIT 14
#define MALLOC 15
#define PS 16
#define IPCS 17
#define SLEEP 18

//Video
#define PAINT_PX 860			//Paint white pixel
#define PAINT_PX_COLOR 861		//Paint color pixel
#define PAINT_REKT 862			//Paint REKTangle
#define FILL_REKT 863			//Fill REKTangle
#define PAINT_IMG 864			//Paint image

//Message queues
#define MQ_OPEN 770
#define MQ_CLOSE 771
#define MQ_SEND 772
#define MQ_RECEIVE 773
#define MQ_IS_FULL 774
#define MQ_IS_EMPTY 775



struct createProcessParams_s {

	uint64_t parentPid;
	char *name;
	void *entryPoint;
	uint64_t argc;
	char **argv;
};


#endif