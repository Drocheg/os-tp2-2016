
#ifndef STACK_POSITION
#define STACK_POSITION 0x10000000
#endif

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x2000
#endif

#ifndef STACK_SIZE
#define STACK_SIZE 32768  /* 256MiB divided into 8KiB pages: (0x10000000/0x2000)  */
#endif


static void *pageStack[STACK_SIZE];
static uint16_t stackTop = 0;


void initializeStack() {

	int i = 0;
	while (i < STACK_SIZE) {
		pageStack[i] = (void *) (STACK_POSITION + (i * PAGE_SIZE));
		i++;
	}
}


void pushPage(void *page) {

	if (stackTop > 0 && stackTop < STACK_SIZE) {
		pageStack[--stackTop] = page;
	}

}


void *popPage() {

	if (pageStack < STACK_SIZE) {
		return pageStack[i++];
	}
	return null;
}























