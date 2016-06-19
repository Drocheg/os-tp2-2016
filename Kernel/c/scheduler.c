#include <memory.h>
#ifndef MAX_PROCESSES
#define MAX_PROCESSES 32
#endif

typedef enum {RUNNING = 1, WAITING, FINISHED} State;

static uint16_t nextPid;

static uint8_t usedNodes[MAX_PROCESSES];
static void Node current;
static void Node last;
static void *memoryPage;
static uint32_t nodeSize;

struct pcb_t {

	uint16_t pid;
	char name[32];
	void *stackPage;
	void *stackBase; /* TODO: Check if we must use it */
	void *stackTop;
	State state;
}


typedef node_t * Node;

struct node_t {
	pcb_t process;
	uint64_t pageIndex;
	Node next;
};

int initializePCB(Node node, char[32] name, void *stack) {

	node->(process.pid) = nextPid++;
	node->(process.name) = name;
	if ( createStack(&node->(process.stackPage)), &node->(process.stackTop) ) {
		return -1;
	}
	node->(process.stackTop) = (void *)(node->(process.stackPage) + PAGE_SIZE - sizeof(uint64_t));
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
	initializeStack(&(newNode->(process.stackTop)), argc, argv, newNode->(process.name));

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
	pageManager(PUSH_PAGE, &(current->(process.stackBase)) ); //TODO: Check if is well written
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



static int initializeStack(void **stackTop, void *mainFunction, uint64_t argc, char *argv[], char name[32]) {

	int count = 0;

	if (argc <= 0) {
		return -1; /* At least, one parameter with the process name must be passed */
	}

	
	/* Pushes stack base null */
	*stackTop -= sizeof(uint64_t);
	memset(*stackTop, 0, sizeof(uint64_t));
	
	/* Pushes argv strings */
	count = argc - 1;
	while (count >= 0) {
		int length = strlen(argv[count]) + 1; /* Adds NULL termination to length */
		
		/* Word alignment */
		int aux = length % sizeof(uint64_t);
		if (aux != 0) {
			length += (sizeof(uint64_t) - aux);
		}

		*stackTop -= (length / sizeof(uint64_t)) * sizeof(uint64_t);
		memcpy(*stackTop, (void*) argv[count], length); /* copies the NULL termination */
		argv[count] = (char*) *stackTop;
		count--;
	}

	/* Pushes end of argv null */
	*stackTop -= sizeof(uint64_t);
	memset(*stackTop, 0, sizeof(uint64_t));
	 /*Pushes argv pointers */
	count = argc - 1;
	while (count >= 0) {
		*stackTop -= sizeof(char *);
		void *argvArrayInStack = ;
		memcpy(*stackTop, &(argv[count]), sizeof(char *));
		count--;
	}

	/* Pushes argv */
	argv = (char *) *stackTop;
	*stackTop -= sizeof(argv);
	memcpy(*stackTop, argv, sizeof(argv));
	/* Pushes argc */
	*stackTop -= sizeof(argc);
	memcpy(*stackTop, &argc, sizeof(argc));

	/* Pushes return address of main */
	// TODO: Lo mandamos a una funcion que sea la que lo pone en estado FINISHED ?

	/* Pushes fake RBP (i.e the stack base of main caller) */
	*stackTop -= sizeof(void *);
	memset(*stackTop, 0, sizeof(void *));

	/* Must push in this order: DS, SS, RSP (new process'), RFLAGS (with a mask), CS, RIP (process main function) */
	/* See: http://stackoverflow.com/questions/6892421/switching-to-user-mode-using-iret */
	/* This must be done in order to start running after iretq */
	/* Values got in https://bitbucket.org/RowDaBoat/wyrm/src */
	/* /d4f3cfcc64325efb1f7d7039fc7bc7c7e85777b0/Kernel/Scheduler/Process.cpp?at=master&fileviewer=file-view-default */
	
	/* Pushes process main function direction */
	*stackTop -= sizeof(void *);
	memcpy(*stackTop, mainFunction, sizeof(void *));

	/* Pushes fake registers */
	*stackTop -= 16 * sizeof(uint64_t);
	memset(*stackTop, 0, 16 * sizeof(uint64_t));

	/* NOW IT CAN RUN */

}

static int strlen(char *string) {

	int count = 0;
	if (string == NULL) {
		return -1;
	}
	while (string[count] == 0) {
		count++;
	}
	return count;
}




/* Must call initializePageStack before!! */
void initializeScheduler() {

	int i = 0;
	pageManager(POP_PAGE, &memoryPage); /* Gets a page to store scheduler circular queue */
	nodeSize = sizeof(struct node_t);
	while (i < MAX_PROCESSES) {
		usedNodes[i++] = 0;
	}
	nextPid = 0;
	current = NULL;
	last = NULL;


}


/*
 * Gets next process in the queue.
 * If  next process is running, it returns it's stack.
 * If next process is waiting, it skips it.
 * If next process is finished, it dequeus it and get the following
 */
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

























