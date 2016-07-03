

#include <stdint.h>
#include <keyboard.h>

#define RAW_BUFFER_SIZE 2048

static char rawBuffer[RAW_BUFFER_SIZE];
static uint64_t rawBufferSize = 0;
static uint64_t rawEnqIdx = 0;
static uint64_t rawDeqIdx = 0;





static void rawEnqueueKey(uint8_t value);
static int64_t rawDequeueKey();
static uint64_t isFull();
static uint64_t isEmpty();







int64_t rawAddElement(uint8_t scanCode) {
	rawEnqueueKey(scanCode);
	return scanCode;
}





/* File Management */
int8_t rawReadChar(uint64_t index, char *dest) {
    int64_t result = rawDequeueKey();
    if (result == -1) {
        return -1;
    }
    *dest = (char) result;
    return 1;
}
int8_t rawWriteChar(uint64_t index, char *src) {
    return -1;  /* Unsupported operation */
}
int8_t rawIsFull(uint64_t index) {
    return !isFull();
}
int8_t rawIsEmpty(uint64_t index) {
    return !isEmpty();
}
int8_t rawClose(uint64_t index) {
    return 0;
}












static uint64_t isFull() {
    return (rawBufferSize == RAW_BUFFER_SIZE);
}

static uint64_t isEmpty() {
    return (rawBufferSize == 0);
}

static void rawEnqueueKey(uint8_t value) {

	if (isFull()) {
		return;
	}

	rawBuffer[rawEnqIdx++] = (char) value;
    rawBufferSize++;
	if (rawEnqIdx == RAW_BUFFER_SIZE) {
		rawEnqIdx = 0;
	}
}

static int64_t rawDequeueKey() {

	int64_t result = 0;
	if (isEmpty()) {
		return -1; /* Same position of index can be achieved is full or if is empty */
	}
	result = rawBuffer[rawDeqIdx++];
    rawBufferSize--;
	if (rawDeqIdx == RAW_BUFFER_SIZE) {
		rawDeqIdx = 0;
	}
	return result;
}




