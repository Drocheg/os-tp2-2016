#include <memory.h>
#include <stdint.h>
#include <stddef.h>
#include <video.h>

#ifndef STACK_POSITION
#define STACK_POSITION 0x10000000
#endif

#ifndef STACK_SIZE
#define STACK_SIZE 32768  /* 256MiB divided into 8KiB pages: (0x10000000/0x2000)  */
#endif


static void *pageStack[STACK_SIZE];
static uint16_t stackTop = 0;
static int initialized = 0;


static void pushPage(void *page);
static void *popPage();


void initializePageStack() {

	uint64_t i = 0;
	while (i < STACK_SIZE) {
		pageStack[i] = (void *) (STACK_POSITION + (i * PAGE_SIZE));
		i++;
	}
	initialized = 1;
}

void pageManager(Action action, void **page) {

	if (initialized) {
		switch (action) {
			case PUSH_PAGE: pushPage(*page); break;
			case POP_PAGE: *page = popPage(); break;
			/* No default because action is an enum and there aren't no more cases */
		}
	}
	return;
}








static void pushPage(void *page) {

	if (stackTop > 0 && stackTop < STACK_SIZE) {
		pageStack[--stackTop] = page;
	}

}


static void *popPage() {

	if (stackTop < STACK_SIZE) {
		return pageStack[stackTop++];
	}
	return NULL;
}

























