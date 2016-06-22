#include <memory.h>
#include <stdlib.h>
#include <interrupts.h>
#include <process.h>
#include <file.h>
#include <stddef.h>

#ifndef MAX_PROCESSES
#define MAX_PROCESSES 0x10 /* See Process.c */
#endif


/* Typedefs*/
typedef enum {RUNNING = 1, WAITING, SLEPT, FINISHED} State;
typedef enum {SLEPT_IO = 1, SLEPT_TIME} SleptState;
typedef uint64_t (*IOActions)(uint64_t, uint64_t);
typedef struct node_t * Node;


/* Structs */
struct node_t {
	uint64_t PCBIndex;
	State state;
	uint64_t generalPurpose1;
	uint64_t generalPurpose2;
	uint64_t generalPurpose3;
	uint64_t index;
	Node next;
};


/* Static variables */
static uint8_t running = 0;						/* Says if the scheduler is on */
static uint8_t usedNodes[MAX_PROCESSES];		/* Holds a list of the nodes, marking those that are occupied*/
static Node last = NULL;						/* Points to the last node in the cirular queue */
static void *memoryPage = NULL;					/* Stores a memory page for the circular queue */
static IOActions ioActions[2];					/* Stores a pointer to actions to be taken on IO request */


/* Static functions prototypes */
static int getFreeNode();




/*********************/
/* Exposed Functions */
/*********************/


/*
 * Sets up the scheduler 
 * Must call initializePageStack!!
 * Returns 0 on sucess, or -1 otherwise
 */
uint64_t initializeScheduler() {

	uint64_t i = 0;
	pageManager(POP_PAGE, &memoryPage); /* Gets a page to store scheduler circular queue */
	if (memoryPage == NULL) {
		return -1;
	}
	while (i < MAX_PROCESSES) {
		usedNodes[i++] = 0;
	}
	return 0;
}

/*
 * Starts the scheduler
 */
void startScheduler() {
	running = 1;
}

/*
 * Stops the scheduler
 */
void stopScheduler() {
	running = 0;
}



/*
 * Updates the process queue, changing to the next process
 * Returns the next process' stack, 
 * or NULL if scheduler is not running or if no process is scheduled
 */
// void *nextProcess(void *currentRSP) {

// 	Node current = NULL;

// 	if (!running || last == NULL) {
// 		return NULL;
// 	}

// 	current = last->next;
// 	current->(process.userStackTop) = currentRSP;
// 	switchToKernelContext(current->kernelStackTop); /* ASM function */

// }


uint64_t waitForIO(uint64_t fileDescriptor, IOOperation ioOperation) {

	Node current = NULL;
	SleptState state;
	if (!running || last == NULL) {
		return -1;
	}
	current = last->next;

	if (fileDescriptor < 0 || fileDescriptor > MAX_FILES || fileDescriptor < getFilesQuantity(current->PCBIndex)) {
		return -1;
	}
	if (ioActions[(uint64_t) ioOperation]) {
		return -1;
	}
	state = SLEPT_IO;
	current->state = SLEPT;
	current->generalPurpose1 = (uint64_t) SLEPT_IO;

}



uint64_t waitForInput(uint64_t PCBIndex, uint64_t fileDescriptor) {



}


uint64_t waitForOutput(uint64_t PCBIndex, uint64_t fileDescriptor) {


}




/*********************/
/* Static Functions */
/*********************/


/* 
 * Enqueus a new process into the queue
 * Returns 0 if everything was OK
 * Returns -1 if no space for the process in the processes list
 * Returns -2 if process couldn't be created
 */
int enqueueProcess(uint64_t parentPid, char name[MAX_NAME_LENGTH], 
	void *entryPoint, uint64_t argc, char *argv[]) {

	int index = -1;
	Node newNode = NULL;	
	Node aux = NULL;
	uint64_t PCBIndex;
	
	if ((index = getFreeNode()) < 0) {
		return -1;
	}
	PCBIndex = createProcess(parentPid, name, entryPoint, argc, argv);

	if (PCBIndex < 0) {
		return -2;
	}

	usedNodes[index] = 1;
	newNode = (Node) (memoryPage + (index * sizeof(*newNode)));
	newNode->PCBIndex = PCBIndex;
	newNode->state = RUNNING;
	newNode->index = index;
	newNode->generalPurpose1 = 0;
	newNode->generalPurpose2 = 0;
	newNode->generalPurpose3 = 0;
	newNode->next = newNode; /* Helps when last is NULL */

	/* attaches the new node into the circular queue */
	aux = (last == NULL) ? newNode : last;
	last = newNode;
	newNode->next = aux->next;
	aux->next = newNode;

	return 0;

}

/* 
 * Takes off the queue the current process (i.e the next one to last).
 * Returns 0 if current process was dequeued, -1 otherwise.
 */
int dequeueProcess() {

	Node current = NULL;

	if (last == NULL) {
		return -1; /* No process to dequeue */
	}

	current = last->next;
	usedNodes[current->index] = 0;
	destroyProcess(current->PCBIndex);
	if (last == last->next) { /* Was the last process */
		last = NULL;
	} else {
		last->next = last->next->next;
	}
	return 0;
}



/* 
 * Returns the first free node index, or -1 if no space.
 * It also puts a mark in the returned index position, so
 * we know that index is now occupied.
 */
static int getFreeNode() {
	
	int i = 0;
	while(i < MAX_PROCESSES && usedNodes[i] == 1) {
		i++;
	}
	if (i < MAX_PROCESSES) {
		usedNodes[i] = 1;
		return i;
	}
	return -1;
}














/*
 * Gets next process in the queue.
 * If  next process is running, it returns it's stack.
 * If next process is waiting, it skips it.
 * If next process is finished, it dequeus it and get the following
 */
 /*
void *nextProcess() {

	Node current = NULL;

	if (last == NULL) {
		return -1;
	}

	Node current = last->next;


	switch (current->(process.state)) {

		case WAITING: last = last->next; return nextProcess;
		case FINISHED: dequeueProcess; break;


	}

	if (current->(process.state) == FINISHED) {
		dequeueProcess();
		return nextProcess();
	}


}

*/























