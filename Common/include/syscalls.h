#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <processCommon.h>

typedef enum {TTY = 0, RAW} KeyboardMode;



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

// Scheduler
#define SLEEP 13
#define WAITPID 14
#define YIELD 15


#define OPENDATAIMGMODULE 20
#define EXIT 21
#define PS 22
#define IPCS 23


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
#define MQ_UNIQUE_NAME 776

//Mutexes
#define MUTEX_LOCK 760
#define MUTEX_UNLOCK 761

//Others
#define CHANGE_KBD_MODE 900




struct createProcessParams_s {

	uint64_t parentPid;
	char *name;
	void *entryPoint;
	uint64_t argc;
	char **argv;
};


#endif