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
typedef enum {RUNNING = 0, SLEPT, FINISHED} State;
typedef enum {SLEPT_IO = 0, SLEPT_TIME} SleptState;
typedef uint64_t (*IOActions)(uint64_t, uint64_t, char **, uint64_t, uint64_t);
typedef uint64_t (*CheckWakeActions)(Node);
typedef uint64_t (*CheckWakeIOActions)();


static char * staticChar[3] = {"R","S","F"};

/* Structs */
struct node_t {
	uint64_t PCBIndex;
	State state;
	uint64_t index;
	Node next;
};


/* Static functions prototypes */
static int getFreeNode();
static uint64_t dequeueProcess();
static uint64_t enqueueProcess(uint64_t parentPid, char name[MAX_NAME_LENGTH], void *entryPoint, uint64_t argc, char *argv[]);
static void *nextProcessRecursive();
static uint64_t checkScheduler();
static int64_t waitForIO(uint64_t fileDescriptor, char *buffer, uint64_t maxBytes, IOOperation ioOperation, uint64_t blockings);
static uint64_t waitForInput(uint64_t PCBIndex, uint64_t fd, char *buffer, uint64_t maxBytes, uint64_t blocking);
static uint64_t waitForOutput(uint64_t PCBIndex, uint64_t fd, char *buffer, uint64_t maxBytes, uint64_t blocking);
static uint64_t waitForTime(uint64_t miliseconds);


/* Static variables */
static uint8_t running = 0;										/* Says if the scheduler is on */
static uint8_t firstIteration = 1;								/* Indicates if is the first time the scheduler is giving the next process */
static uint8_t usedNodes[MAX_PROCESSES];						/* Holds a list of the nodes, marking those that are occupied */
static Node last = NULL;										/* Points to the last node in the cirular queue */
static void *memoryPage = NULL;									/* Stores a memory page for the circular queue */
static IOActions ioActions[2] = {waitForInput, waitForOutput};	/* Stores a pointer to actions to be taken on IO request */






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
		ncPrint("Can't enqueue process");
		for(int i=0; i<100000000;i++);	
		return -1;
	}
	return 0;
}


/*
 * Makes the scheduler be in charge of an input/output operation of the current process
 * This function will read/write <maxBytes> from/into the file mapped in the process' file table by <fileDescriptor>,
 * into/from <buffer>.
 * It's a blocking function, so it will return when maxBytes are achieved
 * Returns read/written bytes, or -1 if any error ocurred
 */
int64_t fileOperation(uint64_t fileDescriptor, char *buffer, uint64_t maxBytes, IOOperation ioOperation, uint64_t blocking) {
	// ncPrint("\nAt fileOperation");
	// int64_t la = waitForIO(fileDescriptor, buffer, maxBytes, ioOperation, blocking);
	// ncPrint("waitForIO returned ");
	// ncPrintDec(la);
	// return la;
	return waitForIO(fileDescriptor, buffer, maxBytes, ioOperation, blocking);
}


/*
 * Stops execution of the running process for <miliseconds> miliseconds.
 * Returns 0 on success (made a <miliseconds> long pause), or -1 otherwise.
 */
uint64_t sleep(uint64_t miliseconds) {

	return waitForTime(miliseconds);
}


/*
 * Updates the process queue, changing to the next process
 * Returns the next process' stack, 
 * or NULL if scheduler is not running or if no process is scheduled
 */
void *nextProcess(void *currentRSP) {
	// ncPrint("I'm here");
	Node current = NULL;
	if (checkScheduler()) {	
		ncPrint("No scheduler");
		for(int i=0; i<100000000;i++);
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
	if (last == last->next) { /* Was the last process */ 
		last = NULL;
	} else {
		last->next = last->next->next;
	}
	destroyProcess(current->PCBIndex);
	
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


static int64_t contador=0;

static void *nextProcessRecursive() {

	Node current = last->next;

	//for(int i=0; i<100000000;i++);
	
	// ncPrintHex(current->state);
	// while(1);

	// if (current->state == RUNNING || current->state == SLEPT) {
	// 	return getProcessStack(current->PCBIndex);
	// }
	if (current->state == FINISHED) {
		dequeueProcess();
		return nextProcessRecursive();
	}
	// if (current->state == SLEPT) {
	// 	last = last->next;
	// 	return nextProcessRecursive();
	// }
	return getProcessStack(current->PCBIndex);
	// return NULL;
		
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


/*
 * Makes the scheduler be in charge of an input/output operation of the current process
 * This function will read/write <maxBytes> from/into the file mapped in the process' file table by <fileDescriptor>,
 * into/from <buffer>.
 * It's a blocking function, so it will return when maxBytes are achieved
 * Returns read/written bytes, or -1 if any error ocurred
 */
static int64_t waitForIO(uint64_t fileDescriptor, char *buffer, uint64_t maxBytes, IOOperation ioOperation, uint64_t blocking) {

	
	Node current = (Node) NULL;
	uint64_t result = 0;
	if (checkScheduler()) {
		return -1;
	}
	current = last->next;

	if (fileDescriptor < 0 || fileDescriptor > MAX_FILES || !existsFile(current->PCBIndex, fileDescriptor)) {
		return -1;
	}
	current->state = SLEPT;

	result = ((ioActions[(uint64_t) ioOperation])(current->PCBIndex, fileDescriptor, buffer, maxBytes, blocking));
	current->state = RUNNING;
	return result;
}


/*
 * Makes the process enter a loop that will be <miliseconds> long.
 * On each turn of the process, it will check if that time has passed.
 * If it does, it will return; if not, the process will yield
 */
static uint64_t waitForTime(uint64_t miliseconds) {

	Node current = (Node) NULL;
	uint64_t ticksAtStart = ticks();
	uint64_t sleepFor = (uint64_t) ((miliseconds/1000) * getPITfrequency());
	uint64_t elapsed = 0;
	
	if (checkScheduler()) {
		return -1;
	}
	current = last->next;

	current->state = SLEPT;
	while (elapsed <= sleepFor) {
		elapsed = (ticks() - ticksAtStart);
		yield();
	}
	current->state = RUNNING;
	
	return 0;
}



static uint64_t waitForInput(uint64_t PCBIndex, uint64_t fd, char *buffer, uint64_t maxBytes, uint64_t blocking) {

	

	uint64_t readData = 0;
	while (readData <= maxBytes) {

		if (operateFile(PCBIndex, fd, IS_EMPTY, NULL) == 0) {
			if (blocking){
				yield();
			} else {
				break;
			}
		} else {
			char c = 0;
			if (operateFile(PCBIndex, fd, READ, &c)) {
				break;
			}
			buffer[readData] = c;
			readData++;
		}
	}

	return readData;
}

static uint64_t waitForOutput(uint64_t PCBIndex, uint64_t fd, char *buffer, uint64_t maxBytes, uint64_t blocking) {
	uint64_t writtenData = 0;
	// ncPrint("\nAt waitForOutput ");
	while (writtenData <= maxBytes) {
		// int64_t la = operateFile(PCBIndex, fd, IS_FULL, NULL);
		// ncPrint("operateFile returned ");
		// ncPrintDec(la);
		if (/*la == 0*/operateFile(PCBIndex, fd, IS_FULL, NULL) == 0) {
			if (blocking){
				yield();
			} else {
				break;
			}
		} else {
			char c = buffer[writtenData];
			if (operateFile(PCBIndex, fd, WRITE, &c)) {
				break;
			}
			writtenData++;
		}
	}
	return writtenData;
}

void printPS() {
	int i = 0;
	while(i < MAX_PROCESSES) {
		if(usedNodes[i] == 1){
			Node newNode = (Node) (memoryPage + (i * sizeof(*newNode)));
			
			ncPrint(" PID:");
			uint64_t newPCBIndex = 	newNode->PCBIndex;
			ncPrintDec(getProcessPID(newPCBIndex));
			ncPrint(" Name: ");
			ncPrint(getProcessName(newPCBIndex));
			ncPrint(" State:");
			uint64_t newState = 	newNode->state;
			ncPrint(staticChar[newNode->state]);
			ncPrint("\n");
			ncPrint(" Memory: ");
			ncPrintDec(getProcessMemoryAmount(newPCBIndex));
			ncPrint(" Next PBCindex: ");
			ncPrintDec(newNode->next->PCBIndex);
			ncPrint("StackPAge");
			ncPrintHex(getProcessStackPage(newPCBIndex));
			ncPrint("StackTop");
			ncPrintHex(getProcessStack(newPCBIndex));
			
		//	newNode->next = newNode; /* Helps when last is NULL */
			ncPrint("\n");
		}

		i++;
	}
	
}
