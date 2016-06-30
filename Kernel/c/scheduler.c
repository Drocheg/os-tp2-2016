#include <memory.h>
#include <stdlib.h>
#include <interrupts.h>
#include <process.h>
// #include <file.h>
#include <fileManager.h>
#include <stddef.h>
#include <time.h>
#include <video.h>


#ifndef MAX_PROCESSES
#define MAX_PROCESSES 0x10 /* See Process.c */
#endif


/* Typedefs*/
typedef struct node_t * Node;
typedef enum {RUNNING = 1, SLEPT, FINISHED} State;
typedef enum {SLEPT_IO = 0, SLEPT_TIME} SleptState;
typedef uint64_t (*IOActions)(Node, char **, uint64_t);
typedef uint64_t (*CheckWakeActions)(Node);
typedef uint64_t (*CheckWakeIOActions)();



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


/* Static functions prototypes */
static int getFreeNode();
static uint64_t dequeueProcess();
static uint64_t enqueueProcess(uint64_t parentPid, char name[MAX_NAME_LENGTH], void *entryPoint, uint64_t argc, char *argv[]);
static void *nextProcessRecursive();
static uint64_t checkScheduler();
static uint64_t waitForIO(uint64_t fileDescriptor, char **buffer, uint64_t maxBytes, IOOperation ioOperation);
static uint64_t waitForInput(Node current, char **buffer, uint64_t maxBytes);
static uint64_t waitForOutput(Node current, char **buffer, uint64_t maxBytes);
static uint64_t waitForTime(uint64_t miliseconds);
static uint64_t checkWake(Node current);
static uint64_t checkWakeIO(Node current);
static uint64_t checkAvailableData(Node current);
static uint64_t checkFreeSpace(Node current);
static uint64_t checkWakeTime(Node current);


/* Static variables */
static uint8_t running = 0;															/* Says if the scheduler is on */
static uint8_t firstIteration = 1;													/* Indicates if is the first time the scheduler is giving the next process */
static uint8_t usedNodes[MAX_PROCESSES];											/* Holds a list of the nodes, marking those that are occupied */
static Node last = NULL;															/* Points to the last node in the cirular queue */
static void *memoryPage = NULL;														/* Stores a memory page for the circular queue */
static IOActions ioActions[2] = {waitForInput, waitForOutput};						/* Stores a pointer to actions to be taken on IO request */
static CheckWakeActions wakeActions[2] = {checkWakeIO, checkWakeTime};				/* Stores a pointer to actions to be taken to check if a process must be waken */
static CheckWakeIOActions wakeIOActions[2] = {checkAvailableData, checkFreeSpace};	/* Stores a pointer to checkers of IO wakening */





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
 * Adds a process into the queue
 * Returns 0 on success, or -1 otherwise
 */
uint64_t addProcess(uint64_t parentPid, char name[MAX_NAME_LENGTH], void *entryPoint, uint64_t argc, char *argv[]) {

	if (enqueueProcess(parentPid, name, entryPoint, argc, argv)) {
		return -1;
	}
	return 0;
}


/*
 * Updates the process queue, changing to the next process
 * Returns the next process' stack, 
 * or NULL if scheduler is not running or if no process is scheduled
 */
void *nextProcess(void *currentRSP) {

	Node current = NULL;
	if (checkScheduler()) {		
		return getKernelStack();
	}
	current = last->next;
	
	if (!firstIteration) {
		setProcessStack(current->PCBIndex, currentRSP);
	} else {	
		firstIteration = 0;
	}
	last = current;		 				/* Change to next process */
	return nextProcessRecursive();

}


/*
 * Returns the current process' PCB index,
 * or -1 if no process scheduled, or schduler not initialzed
 */ 
uint64_t getCurrentPCBIndex() {

	Node current = NULL;
	if (checkScheduler()) {
		return -1;
	}
	current = last->next;

	return current->PCBIndex;
}


/*
 * Finishes the current process
 * Returns 0 on suceess, or -1 otherwise
 */
uint64_t finishProcess() {

	Node current = NULL;
	if (checkScheduler()) {
		return -1;
	}
	current = last->next;

	current->state = FINISHED;
	return 0;
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
static uint64_t enqueueProcess(uint64_t parentPid, char name[MAX_NAME_LENGTH], 
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
static uint64_t dequeueProcess() {

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




static void *nextProcessRecursive() {

	Node current = last->next;

	// ncPrint("I'm here");
	// ncPrintHex(current->state);
	// while(1);

	if (current->state == RUNNING) {
		return getProcessStack(current->PCBIndex);
	}
	if (current->state == FINISHED) {
		dequeueProcess();
		return nextProcessRecursive();
	}
	if (current->state == SLEPT) {
		if (checkWake(current)) {				/* TODO: Check that everything is done here */
			last = last->next;
			return nextProcessRecursive();		/* if checkWake returns -1, the process must remain slept */
		}
		current->state = RUNNING;
		return getProcessStack(current->PCBIndex);
	}


	return NULL;
		
}


/*
 * Checks if the scheduler is running and has processes in its queue
 */
static uint64_t checkScheduler() {

	if (!running || last == NULL) {
		return -1;
	}
	return 0;
}




// static uint64_t waitForIO(uint64_t fileDescriptor, IOOperation ioOperation) {

	
// 	Node current = (Node) NULL;
// 	if (checkScheduler()) {
// 		return -1;
// 	}
// 	current = last->next;

// 	if (fileDescriptor < 0 || fileDescriptor > MAX_FILES || !existsFile(current->PCBIndex, fileDescriptor)) {
// 		return -1;
// 	}
// 	if (ioActions[(uint64_t) ioOperation]) {
// 		return -1;
// 	}
// 	current->state = SLEPT;
// 	current->generalPurpose1 = (uint64_t) SLEPT_IO; 		/* Stores reason of sleep */
// 	current->generalPurpose2 = (uint64_t) ioOperation; 		/* Stores action to take place */
// 	current->generalPurpose3 = (uint64_t) fileDescriptor 	/* Stores file to check */

// 	return 0;
// }


static uint64_t waitForIO(uint64_t fileDescriptor, char **buffer, uint64_t maxBytes, IOOperation ioOperation) {

	
	Node current = (Node) NULL;
	if (checkScheduler()) {
		return -1;
	}
	current = last->next;

	if (fileDescriptor < 0 || fileDescriptor > MAX_FILES || !existsFile(current->PCBIndex, fileDescriptor)) {
		return -1;
	}
	return ((ioActions[(uint64_t) ioOperation])(current, buffer, maxBytes));
	// if (ioActions[(uint64_t) ioOperation]) {
	// 	return -1;
	// }
	// current->state = SLEPT;
	// current->generalPurpose1 = (uint64_t) SLEPT_IO; 		/* Stores reason of sleep */
	// current->generalPurpose2 = (uint64_t) ioOperation; 		/* Stores action to take place */
	// current->generalPurpose3 = (uint64_t) fileDescriptor; 	/* Stores file to check */

	// return 0;
}


// static uint64_t waitForInput(uint64_t PCBIndex, uint64_t fileDescriptor) {

// 	uint64_t flag = getFileFlags(PCBIndex, fileDescriptor) & F_READ;

// 	if (flag != F_READ) {
// 		return -1; /* Permission denied */
// 	}
// 	return 0;
// }


// static uint64_t waitForOutput(uint64_t PCBIndex, uint64_t fileDescriptor) {

// 	uint64_t flag = getFileFlags(PCBIndex, fileDescriptor) & F_WRITE;

// 	if (flag != F_READ) {
// 		return -1; /* Permission denied */
// 	}
// 	return 0;
// }



static uint64_t waitForTime(uint64_t miliseconds) {

	Node current = NULL;
	if (checkScheduler()) {
		return -1;
	}
	current = last->next;

	current->state = SLEPT;
	current->generalPurpose1 = (uint64_t) SLEPT_TIME; 							/* Stores reason of sleep */
	current->generalPurpose2 = (uint64_t)  miliseconds * getPITfrequency();		/* Stores how many ticks must occur to wake up */
	current->generalPurpose3 = ticks();											/* Stores actual amount of ticks */
	return 0;
}



static uint64_t checkWake(Node current) {

	return (wakeActions[current->generalPurpose1])(current);

}


static uint64_t checkWakeIO(Node current) {

	return (wakeIOActions[current->generalPurpose2])(current);

}

// /*
//  * Checks if the file has data available to be read
//  * Returns 0 ig the process must be waken, or -1 otherwise
//  */
// static uint64_t checkAvailableData(Node current) {

// 	File file = (File) current->generalPurpose3;

// 	if (!dataAvailable(file)) {
// 		return -1;
// 	}

// 	/* TODO: What do we do here? */
	
// 	return 0;
// }


/*
 * Checks if the file has data available to be read
 * Returns 0 if the process must be waken, or -1 otherwise
 */
static uint64_t checkAvailableData(Node current) {

	return operateFile(current->PCBIndex, current->generalPurpose3, AVAILABLE_DATA, (char) 0);
}


// /*
//  * Checks if the file has data available to be read
//  * Returns 0 ig the process must be waken, or -1 otherwise
//  */
// static uint64_t checkFreeSpace(Node current) {

// 	File file = (File) current->generalPurpose3;

// 	if (!hasFreeSpace(file)) {
// 		return -1;
// 	}

// 	/* TODO: What do we do here? */
	
// 	return 0;

// }



/*
 * Checks if the file has data available to be read
 * Returns 0 ig the process must be waken, or -1 otherwise
 */
static uint64_t checkFreeSpace(Node current) {

	return operateFile(current->PCBIndex, current->generalPurpose3, FREE_SPACE, (char) 0);
}


/* 
 * Checks if a process must be waken when was sleeping due to time
 * Returns 0 if the process must be waken, or -1 otherwise
 */
static uint64_t checkWakeTime(Node current) {

	uint64_t elapsed = ticks() - current->generalPurpose3;
	uint64_t amount = current->generalPurpose2;
	
	if (elapsed < amount) {
		return -1;
	}
	return 0;

}






uint64_t fileOperation(uint64_t fileDescriptor, char **buffer, uint64_t maxBytes, IOOperation ioOperation) {

	return waitForIO(fileDescriptor, buffer, maxBytes, ioOperation);

}


static uint64_t waitForInput(Node current, char **buffer, uint64_t maxBytes) {

	uint64_t readData = 0;
	while (readData <= maxBytes) {

		if (operateFile(current->PCBIndex, current->generalPurpose3, AVAILABLE_DATA, (char) 0)) {
			yield();
		} else {
			char c = operateFile(current->PCBIndex, current->generalPurpose3, READ, (char) 0);
			if (c == -1) {
				break;
			}
			(*buffer)[readData] = (char) c;
			readData++;
		}
	}
	return readData;
}


static uint64_t waitForOutput(Node current, char **buffer, uint64_t maxBytes) {

	uint64_t writtenData = 0;
	while (writtenData <= maxBytes) {

		if (operateFile(current->PCBIndex, current->generalPurpose3, FREE_SPACE, (char) 0)) {
			yield();
		} else {
			uint64_t result = 0;
			char c = (*buffer)[writtenData];
			result = operateFile(current->PCBIndex, current->generalPurpose3, WRITE, (char) 0);
			if ((char) result != c) {
				break;
			}
			writtenData++;
		}
	}
	return writtenData;

}











