#include <memory.h>
#include <kernel-lib.h>
#include <fileManager.h>
#include <keyboard.h>
#include <mq.h>
#include <basicFile.h>

//TODO implement these where appropriate and delete them from here
int8_t stdinClose(uint64_t index) {
	return -1;
}

int8_t stdoutReadChar(uint64_t index, char *character) {
	return 0;
}

int8_t stdoutWriteChar(uint64_t index, char *character) {
	return 0;
}

int8_t stdoutDataAvailable(uint64_t index) {
	return 0;
}

int8_t stdoutHasFreeSpace(uint64_t index) {
	return 0;
}

int8_t stdoutClose(uint64_t index) {
	return -1;
}

int8_t stderrReadChar(uint64_t index, char *character) {
	return 0;
}

int8_t stderrWriteChar(uint64_t index, char *character) {
	return 0;
}

int8_t stderrDataAvailable(uint64_t index) {
	return 0;
}

int8_t stderrHasFreeSpace(uint64_t index) {
	return 0;
}

int8_t stderrClose(uint64_t index) {
	return -1;
}


/* Structs */
struct fileOperator_s {
	readCharFn_t readCharFn;
	writeCharFn_t writeCharFn;
	isEmptyFn_t isEmptyFn;
	isFullFn_t isFullFn;
	closeFn_t closeFn;
};


/* Static variables */
static uint8_t initialized = 0;
static struct fileOperator_s fileOperators[MESSAGE_QUEUE - STDIN_ + 1];


/* Static functions prototypes */
static int8_t isValidFileType(FileType fileType);
static void initializeSTDIN();
static void initializeSTDOUT();
static void initializeSTDERR();
static void initializeMessageQueues();
static int8_t readChar(FileType fileType, int32_t fileIndex, char *character);
static int8_t writeChar(FileType fileType, int32_t fileIndex, char *character);
static int8_t isEmpty(FileType fileType, int32_t fileIndex);
static int8_t isFull(FileType fileType, int32_t fileIndex);




/*********************/
/* Exposed Functions */
/*********************/


/*
 * Initializes the file manager
 */
void initializeFileManager() {
	initializeBasicFiles();
	initializeSTDIN();
	initializeSTDOUT();
	initializeSTDERR();
	initializeMessageQueues();
	initialized = 1;
}


/*
 * Make <operation> over <fileType> <index> file. 
 * Parameter <character> is used when operation is READ for storing the result,
 * or WRITE, to store a pointer to the character to be written.
 * It will be used just the first position of the <character> pointer.

 * Returns 0 on success, or -1 otherwise. 
 * Note: for IS_EMPTY and IS_FULL, 0 is returned when true, and -1 when false
 */
uint64_t operate(FileOperation operation, FileType fileType, int64_t fileIndex, char *character) {
	if (isValidFileType(fileType)) {
		return -1;
	}
	ncPrint("At operate");
	switch(operation) {
		case READ: return readChar(fileType, fileIndex, character);
		case WRITE: return writeChar(fileType, fileIndex, character);
		case IS_EMPTY: return isEmpty(fileType, fileIndex);
		case IS_FULL: return isFull(fileType, fileIndex);
		default: return -1;
	}
}







/*********************/
/* Static Functions */
/*********************/

/*
 * Checks <fileType> parameter
 * Returns 0 if everything is OK, or -1 otherwise.
 */
static int8_t isValidFileType(FileType fileType) {
	if (((uint64_t)fileType) > MESSAGE_QUEUE - STDIN_) {
		return -1;
	}
	return 0;
}

static void initializeSTDIN() {
	(fileOperators[STDIN_]).readCharFn = &stdinReadChar;
	(fileOperators[STDIN_]).writeCharFn = &stdinWriteChar;
	(fileOperators[STDIN_]).isEmptyFn = &stdinIsEmpty;
	(fileOperators[STDIN_]).isFullFn = &stdinIsFull;
	(fileOperators[STDIN_]).closeFn = &stdinClose;
}

static void initializeSTDOUT() {
	(fileOperators[STDOUT_]).readCharFn = &stdoutReadChar;
	(fileOperators[STDOUT_]).writeCharFn = &stdoutWriteChar;
	(fileOperators[STDOUT_]).isEmptyFn = &stdoutDataAvailable;
	(fileOperators[STDOUT_]).isFullFn = &stdoutHasFreeSpace;
	(fileOperators[STDOUT_]).closeFn = &stdoutClose;
}

static void initializeSTDERR() {
	(fileOperators[STDERR_]).readCharFn = &stderrReadChar;
	(fileOperators[STDERR_]).writeCharFn = &stderrWriteChar;
	(fileOperators[STDERR_]).isEmptyFn = &stderrDataAvailable;
	(fileOperators[STDERR_]).isFullFn = &stderrHasFreeSpace;
	(fileOperators[STDERR_]).closeFn = &stderrClose;
}

static void initializeMessageQueues() {
	(fileOperators[MESSAGE_QUEUE]).readCharFn = &MQreadChar;
	(fileOperators[MESSAGE_QUEUE]).writeCharFn = &MQwriteChar;
	(fileOperators[MESSAGE_QUEUE]).isEmptyFn = &MQisEmpty;
	(fileOperators[MESSAGE_QUEUE]).isFullFn = &MQisFull;
	(fileOperators[MESSAGE_QUEUE]).closeFn = &MQclose;
}

static int8_t readChar(FileType fileType, int32_t fileIndex, char *character) {
	return (fileOperators[(uint64_t) fileType]).readCharFn(fileIndex, character);
}

static int8_t writeChar(FileType fileType, int32_t fileIndex, char *character) {
	return (fileOperators[(uint64_t) fileType]).writeCharFn(fileIndex, character);
}

static int8_t isFull(FileType fileType, int32_t fileIndex) {
	return (fileOperators[(uint64_t) fileType]).isFullFn(fileIndex);
}

static int8_t isEmpty(FileType fileType, int32_t fileIndex) {
	return (fileOperators[(uint64_t) fileType]).isEmptyFn(fileIndex);
}
