#include <tty.h>

#ifndef STDIN_SIZE
#define STDIN_SIZE 1024
#endif



static char stdinBuffer[STDIN_SIZE];

static int64_t enqIdx = 0;
static int64_t deqIdx = 0;
static isFull = 0;




void enqueueChar(int64_t value) {

	if (isFull) {
		return;
	}

	stdinBuffer[enqIdx++] = (char) value;
	if (enqIdx == STDIN_SIZE) {
		enqIdx = 0;
	}
	isFull = (enqIdx == deqIdx);
	return;
}


int64_t dequeueChar() {

	char c;
	if (enqIdx == deqIdx && !isFull) {
		return -1; /* Same position of index can be achieved is full or if is empty */
	}
	c = stdinBuffer[deqIdx++];
	if (deqIdx == STDIN_SIZE) {
		deqIdx = 0;
	}
	return c;
}









