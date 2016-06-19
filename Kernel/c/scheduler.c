#include <memory.h>
#include <stdlib.h>
#include <interrupts.h>

#ifndef MAX_PROCESSES
#define MAX_PROCESSES 32
#endif

/* Typedefs*/
typedef enum {RUNNING = 1, WAITING, FINISHED} State;
typedef node_t * Node;


/*Structs */
struct pcb_t {

	uint16_t pid;
	char name[32];
	void *userStackPage;
	void *userStackTop;
	void *kernelStackPage;
	void *kernelStackTop;
	State state;
}

struct node_t {
	pcb_t process;
	uint64_t pageIndex;
	Node next;
};


/* Static variables */
static uint8_t running;							/* Says if the scheduler is running */
static uint16_t nextPid;						/* Stores the value of the next PID that will be assigned */
static uint8_t usedNodes[MAX_PROCESSES];		/* Holds a list of the nodes, marking those that are occupied*/
static Node last;								/* Points to the last node in the cirular queue */
static void *memoryPage;						/* Stores a memory page for the circular queue */


/* Static functions prototypes */
static int initializePCB(Node node, char[32] name, void *stack);
static int getFreeNode();
static int createStack(void **userStackPage, void **userStackTop);
static int initializeStack(void **userStackTop, void *mainFunction, uint64_t argc, char *argv[], char name[32]);



/*********************/
/* Exposed Functions */
/*********************/


/*
 * Sets up the scheduler 
 * Must call initializePageStack!!
 */
void initializeScheduler() {

	uint64_t i = 0;
	pageManager(POP_PAGE, &memoryPage); /* Gets a page to store scheduler circular queue */
	while (i < MAX_PROCESSES) {
		usedNodes[i++] = 0;
	}
	running = 0;
	nextPid = 0;
	last = NULL;
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
void *nextProcess(void *currentRSP) {

	Node current = NULL;

	if (!running || last == NULL) {
		return NULL;
	}

	current = last->next;
	current->(process.userStackTop) = currentRSP;
	switchToKernelContext(current->kernelStackTop); /* ASM function */

}







/*********************/
/* Static Functions */
/*********************/


static int initializePCB(Node node, char[32] name, void *stack) {

	node->(process.pid) = nextPid++;
	node->(process.name) = name;
	if ( createStack(&node->(process.userStackPage)), &node->(process.userStackTop) ) {
		return -1;
	}
	if ( createStack(&node->(process.kernelStackPage)), &node->(process.kernelStackTop) ) {
		return -1;
	}
	node->(process.state) = RUNNING;
	return 0;
}



/* 
 * Enqueus a new process into the queue
 * Returns 0 if everything was OK
 * Returns -1 if no space for the process in the processes list
 * Returns -2 if there was no space to allocate a stack for the process
 * Returns -3 if there was a problem with parameters
 */
int enqueueProcess(char[32] name, uint64_t argc, char *argv[]) {

	int index = -1;
	Node newNode = NULL;
	void *stack = NULL;	
	Node aux = NULL;
	
	if ((index = getFreeNode()) < 0) {
		return -1;
	}

	newNode = (Node) (memoryPage + (index * sizeof(*newNode)));
	if (initializePCB(newNode, name, stack)) {
		return -2;
	}
	newNode->pageIndex = index;
	newNode->next = newNode; /* Helps when last is NULL */
	if (argc <= 0) {
		return -3
	}
	initializeStack(&(newNode->(process.userStackTop)), argc, argv, newNode->(process.name));

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
	pageManager(PUSH_PAGE, &(current->(process.userStackPage)) ); //TODO: Check if is well written
	if (last == last->next) {
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




static int createStack(void **stackPage, void **stackTop) {

	pageManager(POP_PAGE, stackPage);
	if (*stackPage == NULL) {
		return -1;
	}
	*stackTop = (void *) ((*stackPage) + PAGE_SIZE);
	return 0;
}



/* 
 * Loads stack in order to start running 
 * Returns 0 on success, or -1 otherwise
 */
static int initializeStack(void **userStackTop, void *mainFunction, uint64_t argc, char *argv[], char name[32]) {

	uint64_t count = 0;
	uint64_t processSS = 0;
	uint64_t processRSP = 0;
	uint64_t processRFLAGS = 0x202; /* WYRM value (sets IF and PF) */
	uint64_t processCS = 0x008; /* WYRM value (TODO: Ask Rodrigo) */


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
		argv[count] = (char*) *userStackTop;
		count--;
	}

	/* Pushes end of argv null */
	*userStackTop -= sizeof(uint64_t);
	memset(*userStackTop, 0, sizeof(uint64_t));
	 /*Pushes argv pointers */
	count = argc - 1;
	while (count >= 0) {
		*userStackTop -= sizeof(char *);
		void *argvArrayInStack = ;
		memcpy(*userStackTop, &(argv[count]), sizeof(char *));
		count--;
	}

	/* Pushes argv */
	argv = (char *) *userStackTop;
	*userStackTop -= sizeof(argv);
	memcpy(*userStackTop, argv, sizeof(argv));
	/* Pushes argc */
	*userStackTop -= sizeof(argc);
	memcpy(*userStackTop, &argc, sizeof(argc));

	/* Pushes return address of main */
	// TODO: Lo mandamos a una funcion que sea la que lo pone en estado FINISHED ?

	/* Pushes fake RBP (i.e the stack base of main caller) */
	*userStackTop -= sizeof(uint64_t);
	memset(*userStackTop, 0, sizeof(void *));

	/* Up to here we have the process stack*/
	processRSP = (uint64_t) *userStackTop;

	/* Now we have to push registers values in order to start running after iretq */
	/* Must push in this order: SS, RSP (new process'), RFLAGS (with a mask), CS, RIP */
	/* For more info., see Intel® 64 and IA-32 Architectures Software Developers Manual, Vol. 3-A, Fig. 6-8 */
	

	/* Values got in https://bitbucket.org/RowDaBoat/wyrm/src */
	/* /d4f3cfcc64325efb1f7d7039fc7bc7c7e85777b0/Kernel/Scheduler/Process.cpp?at=master&fileviewer=file-view-default */
	
	/* Pushes SS register */
	*userStackTop -= sizeof(processSS);
	memcpy(*userStackTop, &processSS, sizeof(processSS));

	/* Pushes RSP */
	userStackTop -= sizeof(processRSP);
	memcpy(*userStackTop, &processRSP, sizeof(processRSP));

	/* Pushes RFLAGS */
	userStackTop -= sizeof(processRFLAGS);
	memcpy(*userStackTop, &processRFLAGS, sizeof(processRFLAGS));

	/* Pushes CS */
	userStackTop -= sizeof(processCS);
	memcpy(*userStackTop, &processCS, sizeof(processCS));		

	/* Pushes RIP (i.e: process main function direction or entry point) */
	*userStackTop -= sizeof(uint64_t);
	memcpy(*userStackTop, mainFunction, sizeof(uint64_t));

	/* Pushes fake registers */
	*userStackTop -= 17 * sizeof(uint64_t);
	memset(*userStackTop, 0, 17 * sizeof(uint64_t));

	/* NOW IT CAN RUN */
	return 0;
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























