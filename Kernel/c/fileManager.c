#include <memory.h>
#include <kernel-lib.h>
#include <fileManager.h>




uint64_t stdinReadChar(uint32_t index) {
	return 0;
}

uint64_t stdinWriteChar(uint32_t index, char c) {
	return 0;
}

uint64_t stdinDataAvailable(uint32_t index) {
	return 0;
}

uint64_t stdinHasFreeSpace(uint32_t index) {
	return 0;
}


uint64_t stdoutReadChar(uint32_t index) {
	return 0;
}

uint64_t stdoutWriteChar(uint32_t index, char c) {
	return 0;
}

uint64_t stdoutDataAvailable(uint32_t index) {
	return 0;
}

uint64_t stdoutHasFreeSpace(uint32_t index) {
	return 0;
}

uint64_t stderrReadChar(uint32_t index) {
	return 0;
}

uint64_t stderrWriteChar(uint32_t index, char c) {
	return 0;
}

uint64_t stderrDataAvailable(uint32_t index) {
	return 0;
}

uint64_t stderrHasFreeSpace(uint32_t index) {
	return 0;
}

uint64_t msqsReadChar(uint32_t index) {
	return 0;
}

uint64_t msqsWriteChar(uint32_t index, char c) {
	return 0;
}

uint64_t msqsDataAvailable(uint32_t index) {
	return 0;
}

uint64_t msqsHasFreeSpace(uint32_t index) {
	return 0;
}




/* Typedefs */

/*
 * We assume that every "file" is saved into a table.
 * If a file is unique (for example, video memory or keyboard buffer),
 * a wrapper function must be implemented.
 */
typedef uint64_t (*ReadCharFn)(int32_t index);
typedef uint64_t (*WriteCharFn)(char c, int32_t index);
typedef uint64_t (*DataAvailableFn)(int32_t index);
typedef uint64_t (*HasFreeSpaceFn)(int32_t index);


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
static uint64_t readChar(FileType fileType, int32_t fileIndex);
static uint64_t writeChar(FileType fileType, int32_t fileIndex, char c);
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
 * Make <operation> over <fileType> <index> file. Parameter <c> is used when operation is WRITE
 * Returns non-negative integer on success, or -1 otherwise.
 * 
 * Note: for AVAILABLE_DATA and FREE_SPACE, 0 is returned when true, and -1 when false
 */
uint64_t operate(FileOperation operation, FileType fileType, int64_t fileIndex, char c) {

	if (checkParameters(fileType)) {
		return -1;
	}
	switch(operation) {

		case READ: return readChar(fileType, fileIndex);
		case WRITE: return writeChar(fileType, fileIndex, c);
		case AVAILABLE_DATA: dataAvailable(fileType, fileIndex);
		case FREE_SPACE: hasFreeSpace(fileType, fileIndex);
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

static uint64_t readChar(FileType fileType, int32_t fileIndex) {
	return (fileManagerTable[(uint64_t) fileType]).readCharFn(fileIndex);
}

static uint64_t writeChar(FileType fileType, int32_t fileIndex, char c) {
	return (fileManagerTable[(uint64_t) fileType]).writeCharFn(fileIndex, c);
}

static uint64_t dataAvailable(FileType fileType, int32_t fileIndex) {
	return (fileManagerTable[(uint64_t) fileType]).dataAvailableFn(fileIndex);
}

static uint64_t hasFreeSpace(FileType fileType, int32_t fileIndex) {
	return (fileManagerTable[(uint64_t) fileType]).hasFreeSpaceFn(fileIndex);
}

















