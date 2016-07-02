#include <memory.h>
#include <kernel-lib.h>
#include <fileManager.h>
#include <keyboard.h>
#include <tty.h>
#include <mq.h>
#include <basicFile.h>



//TODO implement these where appropriate and delete them from here
int8_t stdinClose(uint64_t index) {
	return -1;
}

int8_t stdoutClose(uint64_t index) {
	return -1;
}

int8_t stderrReadChar(uint64_t index, char *character) {
	return -1; //0;
}

int8_t stderrWriteChar(uint64_t index, char *character) {
	return -1; //0;
}

int8_t stderrIsEmpty(uint64_t index) {
	return -1; //0;
}

int8_t stderrIsFull(uint64_t index) {
	return -1; //0;
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
static struct fileOperator_s fileOperators[RAW_KEYS - STDIN_ + 1];


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
static int8_t close(FileType fileType, int32_t fileIndex);




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
int64_t operate(FileOperation operation, FileType fileType, int64_t fileIndex, char *character) {
	if (isValidFileType(fileType)) {
		return -1;
	}
	int64_t result;
	int8_t operationResult;
	switch(operation) {
		case READ:
			operationResult = readChar(fileType, fileIndex, character);
			result = operationResult == -1 ? -1 : 0;
			break;
		case WRITE:
			operationResult = writeChar(fileType, fileIndex, character);
			result = operationResult == -1 ? -1 : 0;
			break;
		case IS_EMPTY:
			operationResult = isEmpty(fileType, fileIndex);
			result = operationResult ? 0 : -1;
			break;
		case IS_FULL:
			operationResult = isFull(fileType, fileIndex);
			result = operationResult ? 0 : -1;
			break;
		case CLOSE:
			operationResult = close(fileType, fileIndex);
			result = operationResult == -1 ? -1 : 0;
		default:
			result = -1;
			break;
	}
	return result;
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
	(fileOperators[STDIN_]).readCharFn = stdinReadChar;
	(fileOperators[STDIN_]).writeCharFn = stdinWriteChar;
	(fileOperators[STDIN_]).isEmptyFn = stdinIsEmpty;
	(fileOperators[STDIN_]).isFullFn = stdinIsFull;
	(fileOperators[STDIN_]).closeFn = stdinClose;
}

static void initializeSTDOUT() {
	(fileOperators[STDOUT_]).readCharFn = stdoutReadChar;
	(fileOperators[STDOUT_]).writeCharFn = stdoutWriteChar;
	(fileOperators[STDOUT_]).isEmptyFn = stdoutIsEmpty;
	(fileOperators[STDOUT_]).isFullFn = stdoutIsFull;
	(fileOperators[STDOUT_]).closeFn = stdoutClose;
}

static void initializeSTDERR() {
	(fileOperators[STDERR_]).readCharFn = stderrReadChar;
	(fileOperators[STDERR_]).writeCharFn = stderrWriteChar;
	(fileOperators[STDERR_]).isEmptyFn = stderrIsEmpty;
	(fileOperators[STDERR_]).isFullFn = stderrIsFull;
	(fileOperators[STDERR_]).closeFn = stderrClose;
}

static void initializeMessageQueues() {
	(fileOperators[MESSAGE_QUEUE]).readCharFn = MQreadChar;
	(fileOperators[MESSAGE_QUEUE]).writeCharFn = MQwriteChar;
	(fileOperators[MESSAGE_QUEUE]).isEmptyFn = MQisEmpty;
	(fileOperators[MESSAGE_QUEUE]).isFullFn = MQisFull;
	(fileOperators[MESSAGE_QUEUE]).closeFn = MQclose;
}

static int8_t readChar(FileType fileType, int32_t fileIndex, char *character) {
	return (fileOperators[(uint64_t) fileType]).readCharFn(fileIndex, character);
}

static int8_t writeChar(FileType fileType, int32_t fileIndex, char *character) {
	return (fileOperators[fileType]).writeCharFn(fileIndex, character);
}

static int8_t isFull(FileType fileType, int32_t fileIndex) {
	return (fileOperators[(uint64_t) fileType]).isFullFn(fileIndex);
}

static int8_t isEmpty(FileType fileType, int32_t fileIndex) {
	return (fileOperators[(uint64_t) fileType]).isEmptyFn(fileIndex);
}

static int8_t close(FileType fileType, int32_t fileIndex) {
	return (fileOperators[(uint64_t) fileType]).closeFn(fileIndex);
}
