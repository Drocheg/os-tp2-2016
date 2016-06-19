
static void *interruptsStackTop = NULL;
static void *interruptsStackPage = NULL;

uint64_t initializeInterruptsStack() {
	
	pageManager(POP_PAGE, &interruptsStackPage);
	if (interruptsStackPage == NULL) {
		return -1;
	}
	interruptsStackTop = interruptsStackPage + PAGE_SIZE - sizeof(uint64_t);
	memset(interruptsStackTop, 0, sizeof(uint64_t)); /* NULL terminates the interrupts stack */
	return 0;
}


void *getInterruptsStackTop() {
	return interruptsStackTop;
}