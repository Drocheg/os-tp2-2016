#include <memory.h>
#include <stdlib.h>
#include <process.h>
#include <stddef.h>
#include <kernel-lib.h>
#include <scheduler.h>
#include <process.h>
#include <video.h>



/* Structs */
struct fileDescriptorMapEntry_s {
	File file;
	uint64_t flags;
};


struct fileDescriptorsMap_s {

	uint64_t size;
	struct fileDescriptorMapEntry_s entries[MAX_FILES];
};


struct pcbEntry_s {

	/* Occupied entry */
	uint64_t occupied;

	/* Identification data */
	uint64_t pid;
	uint64_t parentPid;
	char name[MAX_NAME_LENGTH];

	/* State data */
	void *stack;


	/* Control data */
	struct fileDescriptorsMap_s fileDescriptors;


	/* Other data */
	void *stackPage;

	/* Heap */
	void *heapPage;

	/* Free Space to reach 512 Bytes (when adding a field, bytes should be taken from here)  */
	uint64_t freeSpace[21];
};





/* Static vars */
static void *pcbMemoryPage = NULL;
static uint64_t maxProcesses = 0;
static uint64_t nextPid = 1;
static struct pcbEntry_s *pcb; /* Easier to acccess data in pcb */


/* Static functions prototypes */
static uint64_t createStack(void **stackPage, void **stackTop);
static uint64_t initializeStack(void **userStackTop, char name[32], void *mainFunction, uint64_t argc, char *argv[]);
static void *mallocRecursive(void **current, void *next, uint64_t size);


/* 
 * Returns a pointer to the next position with <size> bytes available in a page of the heap,
 * or NULL if no more space available, pcb not initialize, or illegal arguments
 */
void *malloc(uint64_t PCBIndex, int64_t size) {

	struct pcbEntry_s *process = NULL;
	if (pcb == NULL || PCBIndex < 0 || PCBIndex > maxProcesses || size < 0 || size>PAGE_SIZE - (sizeof(void *) + sizeof(uint64_t)) ) {
		return NULL;
	}
	process = &(pcb[PCBIndex]);

	return mallocRecursive(&(process->heapPage), size);
}


static void *mallocRecursive(void **current, uint64_t size) {

	uint64_t *currentSize;

	if ((uint64_t *) *current == NULL) {
		pageManager(POP_PAGE, current);
		if (current == NULL) {
			return NULL;
		}
		*((uint64_t *) *current) = (uint64_t) NULL; //Next = null;							/* TODO: Check this */
		*((uint64_t *) (*current + sizeof(void *))) = sizeof(void *) + sizeof(uint64_t); 	/* TODO: Check this */ 
	}

	currentSize = ((uint64_t *) (*current + sizeof(void *)));

	if (size <= PAGE_SIZE-*currentSize) { 
		void *result = *current + (*currentSize);
		*currentSize += size;
		return result;
	}
	return mallocRecursive(*current, size);
}




/*********************/
/* Exposed Functions */
/*********************/



/*
 * Creates a PCB
 * Returns 0 on success, or -1 otherwise
 */
uint64_t initializePCB() {

	pageManager(POP_PAGE, &pcbMemoryPage); /* Gets a page to store scheduler circular queue */
	if (pcbMemoryPage == NULL) {
		return -1;
	}
	maxProcesses = PAGE_SIZE / sizeof(struct pcbEntry_s); /* Should be 0x10 */
	pcb = (struct pcbEntry_s *) pcbMemoryPage;
	memset((void *)pcb, 0, PAGE_SIZE); /* Cleans page (clears occupied field of each entry in pcb) */
	return 0;
}



/*
 * Creates a process and loads it into the PCB
 * Returns process PCB index on success.
 * Returns -1 if PCB wasn't initialized
 * Returns -2 if no space in PCB.
 * Returns -3 if no memory for the stack.
 * Returns -4 if there was a problem with parameters
 */
uint64_t createProcess(uint64_t parentPid, char name[32], void *entryPoint, uint64_t argc, char *argv[]) {

	uint64_t index = 0;
	struct pcbEntry_s *newProcess = NULL;
	if (pcb == NULL) {
		return -1;
	}
	while (index < maxProcesses && pcb[index].occupied != 0) {
		index++;
	}
	if (index == maxProcesses) {
		return -2; /* Couldn't create more processes */
	}

	newProcess = &(pcb[index]);
	newProcess->occupied = 1;
	newProcess->pid = nextPid++;
	newProcess->parentPid = parentPid;
	memcpy(newProcess->name, name, 32);
	if (createStack( &(newProcess->stackPage), &(newProcess->stack))) {
		return -3; /* Couldn't create a stack for the new process */
	}
	if (initializeStack(&(newProcess->stack), name, entryPoint, argc, argv)) {
		return -4; /* Problems with parameters */
	}
	(newProcess->fileDescriptors).size = 0;
	memset((void *)((newProcess->fileDescriptors).entries), 0, MAX_FILES * sizeof(struct fileDescriptorsMap_s));
	newProcess->heapPage = NULL;
	return index;
}



/* Getters */
uint64_t getProcessPID(uint64_t PCBIndex) {

	struct pcbEntry_s *process = NULL;
	if (pcb == NULL || PCBIndex < 0 || PCBIndex > maxProcesses) {
		return -1;
	}
	process = &(pcb[PCBIndex]);
	return process->pid;

}
uint64_t getProcessParentPID(uint64_t PCBIndex) {

	struct pcbEntry_s *process = NULL;
	if (pcb == NULL || PCBIndex < 0 || PCBIndex > maxProcesses) {
		return -1;
	}
	process = &(pcb[PCBIndex]);
	return process->parentPid;
}
char *getProcessName(uint64_t PCBIndex) {

	struct pcbEntry_s *process = NULL;
	if (pcb == NULL || PCBIndex < 0 || PCBIndex > maxProcesses) {
		return NULL;
	}
	process = &(pcb[PCBIndex]);
	return process->name;
}
void *getProcessStack(uint64_t PCBIndex) {
	struct pcbEntry_s *process = NULL;
	if (pcb == NULL || PCBIndex < 0 || PCBIndex > maxProcesses) {
		return NULL;
	}
	process = &(pcb[PCBIndex]);
	return process->stack;
}
uint64_t getFilesQuantity(uint64_t PCBIndex) {

	struct pcbEntry_s *process = NULL;
	if (pcb == NULL || PCBIndex < 0 || PCBIndex > maxProcesses) {
		return -1;
	}
	process = &(pcb[PCBIndex]);
	return (process->fileDescriptors).size;
}
uint64_t getFileDescriptor(uint64_t PCBIndex, File file) {

	struct pcbEntry_s *process = NULL;
	uint64_t index = 0;
	if (pcb == NULL || PCBIndex < 0 || PCBIndex > maxProcesses) {
		return -1;
	}
	process = &(pcb[PCBIndex]);

	while (index < MAX_FILES && (process->fileDescriptors).entries[index].file != file) {
		index++;
	}

	return (index == MAX_FILES) ? -1 : index;
}
uint64_t getFileFlags(uint64_t PCBIndex, uint64_t fileDescriptor) {

	struct pcbEntry_s *process = NULL;
	if (pcb == NULL || PCBIndex < 0 || PCBIndex > maxProcesses) {
		return -1;
	}
	process = &(pcb[PCBIndex]);
	return (((process->fileDescriptors).entries)[fileDescriptor]).flags;
}
File getFile(uint64_t PCBIndex, uint64_t fileDescriptor) {

	struct pcbEntry_s *process = NULL;
	if (pcb == NULL || PCBIndex < 0 || PCBIndex > maxProcesses) {
		return NULL;
	}
	process = &(pcb[PCBIndex]);
	return (((process->fileDescriptors).entries)[fileDescriptor]).file;
}

/* Setters */
uint64_t setProcessStack(uint64_t PCBIndex, void *stack) {

	struct pcbEntry_s *process = NULL;
	if (pcb == NULL || PCBIndex < 0 || PCBIndex > maxProcesses) {
		return -1;
	}
	process = &(pcb[PCBIndex]);
	process->stack = stack;
	return 0;
}
uint64_t setFileFlags(uint64_t PCBIndex, uint64_t fileDescriptor, uint64_t flags) {

	struct pcbEntry_s *process = NULL;
	if (pcb == NULL || PCBIndex < 0 || PCBIndex > maxProcesses) {
		return -1;
	}
	process = &(pcb[PCBIndex]);
	(((process->fileDescriptors).entries)[fileDescriptor]).flags = flags;
	return 0;
}



/* File management */

/*
 * Adds a file into the process opened files list
 * Returns the file descriptor of the file, or -1 otherwise
 */
uint64_t openFile(uint64_t PCBIndex, File file, uint64_t flags) {

	struct pcbEntry_s *process = NULL;
	struct fileDescriptorMapEntry_s *entries;
	uint64_t index = 0;
	if (pcb == NULL || PCBIndex < 0 || PCBIndex > maxProcesses) {
		return -1;
	}
	process = &(pcb[PCBIndex]);
	if ((process->fileDescriptors).size >= MAX_FILES) {
		return -1;
	}
	
	entries = (process->fileDescriptors).entries;
	while (entries[index].file != NULL) {
		index++;
	}

	((process->fileDescriptors).size)++;
	entries[index].file = file;
	entries[index].flags = flags;
	return 0;
}

uint64_t closeFile(uint64_t PCBIndex, File file) {

	struct pcbEntry_s *process = NULL;
	uint64_t index = 0;
	if (pcb == NULL || PCBIndex < 0 || PCBIndex > maxProcesses) {
		return -1;
	}
	process = &(pcb[PCBIndex]);
	index = getFileDescriptor(PCBIndex, file);

	if (index < 0) {
		return -1;
	}

	((process->fileDescriptors).size)--;
	(process->fileDescriptors).entries[index].file = NULL;
	(process->fileDescriptors).entries[index].flags = 0;
	return 0;
}



uint64_t destroyProcess(uint64_t PCBIndex) {

	struct pcbEntry_s *process = NULL;
	if (pcb == NULL || PCBIndex < 0 || PCBIndex > maxProcesses) {
		return -1;
	}

	process = &(pcb[PCBIndex]);
	pageManager(PUSH_PAGE, &(process->stackPage)); /* Returns stack memory page */

	//Free heap


	memset(process, 0, sizeof(*process)); /* Clears the process' pcb entry */
	return 0;
}






/*********************/
/* Static Functions */
/*********************/


/*
 * Function that changes the process state to finished in the scheduler.
 * Returns -1 if there was a problem (i.e. scheduler wasn't on), or does not return otherwise.
 * Brief explanation:
 * This function calls finishProcess() from scheduler, which changes the process' state to FINISHED.
 * When finishProcess() returns, the next instruction is an infinite loop. After that, the scheduler
 * will continue running until the terminated process' turn. When that happens, it will destroy the process.
 * TODO: Check kernel-userland issue
 */
uint64_t terminateProcess() {

	if (finishProcess()) {
		return -1;
	}
	return 0;
}


/*
 * Creates a stack by allocating one memory page and pointing the stack pointer to the end of it.
 * Returns 0 on success, or -1 otherwise.
 */
static uint64_t createStack(void **stackPage, void **stackTop) {

	pageManager(POP_PAGE, stackPage);
	if (*stackPage == NULL) {
		return -1;
	}
	*stackTop = (void *) ((*stackPage) + PAGE_SIZE);
	return 0;
}


/* 
 * Loads stack in order to start running.
 * Returns 0 on success, or -1 otherwise.
 */
static uint64_t initializeStack(void **userStackTop, char name[32], void *mainFunction, uint64_t argc, char *argv[]) {

	int64_t count = 0;
	uint64_t argvPosition = 0;
	uint64_t processSS = 0;
	uint64_t processRSP = 0;
	uint64_t processRFLAGS = 0x202; 					/* WYRM value (sets IF and PF) */
	uint64_t processCS = 0x008;							/* WYRM value (TODO: Ask Rodrigo) */
	uint64_t processRIP = (uint64_t) mainFunction;

	if (argc <= 0) {
		return -1; /* At least, one parameter with the process name must be passed */
	}



	
	/* Pushes stack base null */
	*userStackTop -= sizeof(uint64_t);
	memset(*userStackTop, 0, sizeof(uint64_t));

	/* Pushes argv strings */
	count = argc - 1;

	while (count >= 0) {
		
		int length = strlen(argv[count]) + 1; /* Adds NULL termination to length */
	
		/* Word alignment */
		int aux = length % sizeof(uint64_t);
		if (aux != 0) {
			length += (sizeof(uint64_t) - aux);
		}

		*userStackTop -= (length / sizeof(uint64_t)) * sizeof(uint64_t);
		memcpy(*userStackTop, (void*) argv[count], length); /* copies the NULL termination */
		argv[count] = (char *) *userStackTop;
		count--;

	}

	/* Pushes end of argv null */
	*userStackTop -= sizeof(uint64_t);
	memset(*userStackTop, 0, sizeof(uint64_t));
	 
	 /*Pushes argv pointers */
	count = argc - 1;
	while (count >= 0) {
		*userStackTop -= sizeof(char *);
		memcpy(*userStackTop, &(argv[count]), sizeof(char *));
		count--;
	}
	argv = (char **) *userStackTop;

	/* Pushes return address of main */
	*userStackTop -= sizeof(void *);
	memset(*userStackTop, 0, sizeof(void *));

	/* Pushes fake RBP (i.e the stack base of main caller) */
	*userStackTop -= sizeof(uint64_t);
	memset(*userStackTop, 0, sizeof(void *));

	/* Up to here we have the process stack*/
	processRSP = (uint64_t) *userStackTop;

	/* Now we have to push registers values in order to start running after iretq */
	/* Must push in this order: SS, RSP (new process'), RFLAGS (with a mask), CS, RIP */
	/* For more info., see Intel® 64 and IA-32 Architectures Software Developers Manual, Vol. 3-A, Fig. 6-8 */
	

	/* Values got from https://bitbucket.org/RowDaBoat/wyrm/src */
	/* /d4f3cfcc64325efb1f7d7039fc7bc7c7e85777b0/Kernel/Scheduler/Process.cpp?at=master&fileviewer=file-view-default */
	

	/* Pushes SS register */
	*userStackTop -= sizeof(processSS);
	memcpy(*userStackTop, &processSS, sizeof(processSS));

	/* Pushes RSP */
	*userStackTop -= sizeof(processRSP);
	memcpy(*userStackTop, &processRSP, sizeof(processRSP));

	/* Pushes RFLAGS */
	*userStackTop -= sizeof(processRFLAGS);
	memcpy(*userStackTop, &processRFLAGS, sizeof(processRFLAGS));

	/* Pushes CS */
	*userStackTop -= sizeof(processCS);
	memcpy(*userStackTop, &processCS, sizeof(processCS));

	/* Pushes RIP (i.e: process main function direction or entry point) */
	*userStackTop -= sizeof(processRIP);
	memcpy(*userStackTop, &processRIP, sizeof(processRIP));

	/* Pushes fake registers */
	*userStackTop -= 5 * sizeof(uint64_t);
	memset(*userStackTop, 0, 5 * sizeof(uint64_t));

	/* Pushes argc */
	*userStackTop -= sizeof(argc);
	memcpy(*userStackTop, &argc, sizeof(argc));

	/* Pushes argv */
	*userStackTop -= sizeof(argvPosition);
	memcpy(*userStackTop, &argv, sizeof(argvPosition));

	/* Pushes fake registers */
	*userStackTop -= 10 * sizeof(uint64_t);
	memset(*userStackTop, 0, 10 * sizeof(uint64_t));

	/* NOW IT CAN RUN */
	return 0;
}













