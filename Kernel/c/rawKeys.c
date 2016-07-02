

#include <stdint.h>
#include <keyboard.h>

#define RAW_BUFFER_SIZE 2048

static char rawBuffer[RAW_BUFFER_SIZE];
static uint64_t bufferSize = 0;
static uint64_t enqIdx = 0;
static uint64_t deqIdx = 0;


static void enqueueKey(uint8_t value);
static int64_t dequeueKey();
static uint64_t isFull();
static uint64_t isEmpty();


int64_t rawAddElement(uint8_t scanCode) {
	enqueueKey(scanCode);
	return scanCode;
}



static uint64_t isFull() {
    return (bufferSize == RAW_BUFFER_SIZE);
}

static uint64_t isEmpty() {
    return (bufferSize == 0);
}

static void enqueueKey(uint8_t value) {

	if (isFull()) {
		return;
	}

	rawBuffer[enqIdx++] = (char) value;
    bufferSize++;
	if (enqIdx == RAW_BUFFER_SIZE) {
		enqIdx = 0;
	}
}

static int64_t dequeueKey() {

	int64_t result = 0;
	if (isEmpty()) {
		return -1; /* Same position of index can be achieved is full or if is empty */
	}
	result = rawBuffer[deqIdx++];
    bufferSize--;
	if (deqIdx == RAW_BUFFER_SIZE) {
		deqIdx = 0;
	}
	return result;
}