#include <memory.h>
#include <kernel-lib.h>
#include <fileManager.h>

#include <keyboard.h>
#include <tty.h>




int64_t stderrReadChar(uint32_t index, char *character) {
	return 0;
}

int64_t stderrWriteChar(uint32_t index, char *character) {
	return 0;
}

int64_t stderrDataAvailable(uint32_t index) {
	return 0;
}

int64_t stderrHasFreeSpace(uint32_t index) {
	return 0;
}

int64_t msqsReadChar(uint32_t index, char *character) {
	return 0;
}

int64_t msqsWriteChar(uint32_t index, char *character) {
	return 0;
}

int64_t msqsDataAvailable(uint32_t index) {
	return 0;
}

int64_t msqsHasFreeSpace(uint32_t index) {
	return 0;
}




/* Typedefs */

/*
 * We assume that every "file" is saved into a table.
 * If a file is unique (for example, video memory or keyboard buffer),
 * a wrapper function must be implemented.
 */
typedef int64_t (*ReadCharFn)(int32_t, char*);
typedef int64_t (*WriteCharFn)(int32_t, char*);
typedef int64_t (*DataAvailableFn)(int32_t);
typedef int64_t (*HasFreeSpaceFn)(int32_t);


/* Structs */
struct fileManagerDescriptor_s {

	ReadCharFn readCharFn;
	WriteCharFn writeCharFn;
	DataAvailableFn dataAvailableFn;
	HasFreeSpaceFn hasFreeSpaceFn;
};


/* Static variables */
static uint8_t initialized = 0;
static struct fileManagerDescriptor_s fileManagerTable[MESSAGE_QUEUES - STDIN_ + 1];


/* Static functions prototypes */
static uint64_t checkParameters(FileType fileType);
static void initializeSTDIN();
static void initializeSTDOUT();
static void initializeSTDERR();
static void initializeMessageQueues();
static uint64_t readChar(FileType fileType, int32_t fileIndex, char *character);
static uint64_t writeChar(FileType fileType, int32_t fileIndex, char *character);
static uint64_t dataAvailable(FileType fileType, int32_t fileIndex);
static uint64_t hasFreeSpace(FileType fileType, int32_t fileIndex);




/*********************/
/* Exposed Functions */
/*********************/


/*
 * Initializes the file manager
 */
void initializeFileManager() {


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
 * Note: for AVAILABLE_DATA and FREE_SPACE, 0 is returned when true, and -1 when false
 */
uint64_t operate(FileOperation operation, FileType fileType, int64_t fileIndex, char *character) {


	if (checkParameters(fileType)) {
		return -1;
	}
	switch(operation) {

		case READ: return readChar(fileType, fileIndex, character);
		case WRITE: return writeChar(fileType, fileIndex, character);
		case AVAILABLE_DATA: return dataAvailable(fileType, fileIndex);
		case FREE_SPACE: return hasFreeSpace(fileType, fileIndex);
	}
	return -1;
}







/*********************/
/* Static Functions */
/*********************/

/*
 * Checks <fileType> parameter
 * Returns 0 if everything is OK, or -1 otherwise.
 */
static uint64_t checkParameters(FileType fileType) {

	if (((uint64_t)fileType) > MESSAGE_QUEUES - STDIN_) {
		return -1;
	}
	return 0;
}

static void initializeSTDIN() {

	(fileManagerTable[STDIN_]).readCharFn = stdinReadChar;
	(fileManagerTable[STDIN_]).writeCharFn = stdinWriteChar;
	(fileManagerTable[STDIN_]).dataAvailableFn = stdinDataAvailable;
	(fileManagerTable[STDIN_]).hasFreeSpaceFn = stdinHasFreeSpace;
}

static void initializeSTDOUT() {

	(fileManagerTable[STDOUT_]).readCharFn = stdoutReadChar;
	(fileManagerTable[STDOUT_]).writeCharFn = stdoutWriteChar;
	(fileManagerTable[STDOUT_]).dataAvailableFn = stdoutDataAvailable;
	(fileManagerTable[STDOUT_]).hasFreeSpaceFn = stdoutHasFreeSpace;
}

static void initializeSTDERR() {

	(fileManagerTable[STDERR_]).readCharFn = stderrReadChar;
	(fileManagerTable[STDERR_]).writeCharFn = stderrWriteChar;
	(fileManagerTable[STDERR_]).dataAvailableFn = stderrDataAvailable;
	(fileManagerTable[STDERR_]).hasFreeSpaceFn = stderrHasFreeSpace;
}

static void initializeMessageQueues() {

	(fileManagerTable[MESSAGE_QUEUES]).readCharFn = msqsReadChar;
	(fileManagerTable[MESSAGE_QUEUES]).writeCharFn = msqsWriteChar;
	(fileManagerTable[MESSAGE_QUEUES]).dataAvailableFn = msqsDataAvailable;
	(fileManagerTable[MESSAGE_QUEUES]).hasFreeSpaceFn = msqsHasFreeSpace;
}

static uint64_t readChar(FileType fileType, int32_t fileIndex, char *character) {
	return (fileManagerTable[(uint64_t) fileType]).readCharFn(fileIndex, character);
}

static uint64_t writeChar(FileType fileType, int32_t fileIndex, char *character) {
	return (fileManagerTable[(uint64_t) fileType]).writeCharFn(fileIndex, character);
}

static uint64_t dataAvailable(FileType fileType, int32_t fileIndex) {
	return (fileManagerTable[(uint64_t) fileType]).dataAvailableFn(fileIndex);
}

static uint64_t hasFreeSpace(FileType fileType, int32_t fileIndex) {
	return (fileManagerTable[(uint64_t) fileType]).hasFreeSpaceFn(fileIndex);
}

















