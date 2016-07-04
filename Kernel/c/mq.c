#include <mq.h>
#include <file.h>
#include <basicFile.h>
#include <kernel-lib.h>
#include <stdlib.h>
#include <process.h>
#include <scheduler.h>
#include <fileManager.h>
#include <stdlib.h>
#include <video.h>
#include <mutex.h>

#define MAX_MQS 256

typedef struct {
	BasicFile file;			//File where actual data is read/written
	uint64_t links;			//The number of processes who have this message queue open. Upon reaching 0, it is destroyed.
	int64_t readPID;		//ID of the process who has this MQ open for reading. -1 if none.
	int64_t writePID;		//ID of the process who has this MQ open for writing. -1 if none.
} MessageQueue;

static MessageQueue mqs[MAX_MQS] = {NULL};		//Initialize with NULL
static uint64_t numMQs = 0;
static volatile uint8_t openMutex = 0,
				closeMutex = 0,
				uniqNameMutex = 0;
static uint32_t nextUniqueID = 0;

/**
* @return The index of the message queue with the specified name, or -1 if not found.
*/
static int64_t indexOfMQ(const char* name);

/**
* Creates a new message queue with the given name.
* @return 1 on success, or -1 on error.
*/
static int8_t newMQ(const char* name, uint64_t tableIndex, uint32_t accessFlags);

/**
* @return An index in the MQ table where there is room for a new MQ, or -1 if it's full.
*/
static int64_t findFreeSlot();

/**
* Marks the specified message queue as opened in the specified mode.
* @return 1 on success, -1 on error (e.g. already open in the specified mode)
*/
static int8_t markAccess(uint64_t tableIndex, uint64_t pid, uint32_t accessFlags);

/**
* Destroys the specified message queue and its resources, if it exists.
*
* @return 1 on success, 0 on error.
*/
static int8_t destroyMQ(uint64_t index);


/* **********************************************
*			FileManager implementations
* **********************************************/
int64_t MQopen(const char* name, uint32_t accessFlags) {
	if(name == NULL || ((accessFlags & F_READ) == 0 && (accessFlags & F_WRITE) == 0)) {
		return -1;
	}
	mutex_lock(&openMutex);
	int64_t tableIndex = indexOfMQ(name);
	//Nonexistant MQ, create
	if(tableIndex == -1) {
		tableIndex = findFreeSlot();
		if(tableIndex == -1) {
			mutex_unlock(&openMutex);
			return -1;
		}
		if(newMQ(name, tableIndex, accessFlags) == -1) {
			mutex_unlock(&openMutex);
			return -1;
		}
	}
	//Existint MQ, check priviledges
	else {
		//Only 1 process can have the MQ open for reading/writing at the same time
		if(markAccess(tableIndex, getCurrentPID(), accessFlags) == -1) {
			mutex_unlock(&openMutex);
			return -1;
		}
	}
	//MQ valid, register it in the current process' PCB
	int64_t MQdescriptor = addFile(getCurrentPCBIndex(), tableIndex, MESSAGE_QUEUE, accessFlags);
	if(MQdescriptor == -1) {
		MQclose(tableIndex);
		mutex_unlock(&openMutex);
		return -1;
	}
	mqs[tableIndex].links++;
	mutex_unlock(&openMutex);
	return (int64_t) MQdescriptor;
}

int8_t MQuniq(char *dest) {
	mutex_lock(&uniqNameMutex);
	char name[MAX_NAME+1] = {'/', 'm', 'q', '/', 'u', 0};	//Prefix these MQs with "/mq/u"
	uint32_t startingID = nextUniqueID;
	//Append the next unique ID to the "/mq/u" prefix and check if a MQ with that name exists
	do {
		intToStr((uint64_t)++nextUniqueID, name+5);
		if(nextUniqueID == startingID) {	//Overflowed and came back around with no matches
			mutex_unlock(&uniqNameMutex);
			return -1;
		}
	} while(indexOfMQ(name) != -1);
	memcpy(dest, name, strlen(name)+1);
	mutex_unlock(&uniqNameMutex);
	return 0;
}

int8_t MQclose(uint64_t tableIndex) {
	mutex_lock(&closeMutex);
	if(mqs[tableIndex].file == NULL) {
		mutex_unlock(&closeMutex);
		return -1;
	}
	uint64_t pid = getCurrentPID();
	if(mqs[tableIndex].readPID == pid) {
		mqs[tableIndex].readPID = -1;
	}
	else if(mqs[tableIndex].writePID == pid) {
		mqs[tableIndex].writePID = -1;
	}
	else {
		mutex_unlock(&closeMutex);
		return -1;
	}
	if(mqs[tableIndex].links > 0) {
		mqs[tableIndex].links--;
	}
	if(mqs[tableIndex].links == 0) {		//No more links, destroy
		int8_t destroyResult = destroyMQ(tableIndex);
		mutex_unlock(&closeMutex);
		return destroyResult == 0 ? -1 : destroyResult;
	}
	else {
		mutex_unlock(&closeMutex);
		return 0;
	}
}

int8_t MQreadChar(uint64_t tableIndex, char *dest) {
	if(tableIndex < 0 || tableIndex >= MAX_MQS || mqs[tableIndex].file == NULL || mqs[tableIndex].readPID != getCurrentPID()) {
		return -1;
	}
	*dest = basicFileReadChar(mqs[tableIndex].file);
	return *dest == EOF ? 0 : 1;
}


// int8_t MQreceive(mqd_t descriptor, char *buff, size_t buffLen) {
// 	if(descriptor.accessMode != F_READ || mqs[descriptor.tableIndex] == NULL) {
// 		return -1;
// 	}
// 	File f = mqs[descriptor.tableIndex].file;
// 	size_t readBytes = 0;
// 	for(readBytes = 0; readBytes < buffLen; readBytes++) {
// 		int8_t c = basicFileReadChar(f);
// 		buff[readBytes] = c;
// 		if(c == EOF) {
// 			break;
// 		}
// 	}
// 	return readBytes;
// }

// int8_t MQreceiveNoblock(mqd_t descriptor, char *buff, size_t buffLen) {
// 	if(descriptor.accessMode != F_READ || mqs[descriptor.tableIndex] == NULL) {
// 		return -1;
// 	}
// 	File f = mqs[descriptor.tableIndex].file;
// 	size_t readBytes = 0;
// 	for(readBytes = 0; readBytes < buffLen; readBytes++) {
// 		if(!dataAvailable(f)) {
// 			break;
// 		}
// 		int8_t c = basicFileReadChar(f);
// 		buff[readBytes] = c;
// 		if(c == EOF) {
// 			break;
// 		}
// 	}
// 	return readBytes;
// }

int8_t MQwriteChar(uint64_t tableIndex, char *src) {
	if(tableIndex < 0 || tableIndex >= MAX_MQS || mqs[tableIndex].file == NULL || mqs[tableIndex].writePID != getCurrentPID()) {
		return -1;
	}
	// int8_t la = basicFileWriteChar(*src, mqs[tableIndex].file) == EOF ? 0 : 1;
	// if(!la) {
	// 	ncPrint("MQwriteChar failed");
	// }
	// return la;
	return basicFileWriteChar(*src, mqs[tableIndex].file) == EOF ? 0 : 1;
}

// int8_t MQsend(mqd_t descriptor, const char *msg, size_t mgsLen) {
// 	if(descriptor.accessMode != F_WRITE || mqs[descriptor.tableIndex] == NULL) {
// 		return -1;
// 	}
// 	File f = mqs[descriptor.tableIndex].file;
// 	size_t writtenBytes = 0;
// 	for(writtenBytes = 0; writtenBytes < buffLen; writtenBytes++) {
// 		basicFileWriteChar(msg[writtenBytes], f);
// 	}
// 	return writtenBytes;
// }

// int8_t MQsendNoblock(mqd_t descriptor, const char *msg, size_t mgsLen) {
// 	if(descriptor.accessMode != F_WRITE || mqs[descriptor.tableIndex] == NULL) {
// 		return -1;
// 	}
// 	File f = mqs[descriptor.tableIndex].file;
// 	size_t writtenBytes = 0;
// 	for(writtenBytes = 0; writtenBytes < buffLen; writtenBytes++) {
// 		if(!hasFreeSpace(f)) {
// 			break;
// 		}
// 		basicFileWriteChar(msg[writtenBytes], f);
// 	}
// 	return writtenBytes;
// }

int8_t MQisEmpty(uint64_t index) {
	if(index < 0 || index >= MAX_MQS || mqs[index].file == NULL) {
		return -1;
	}
	return basicFileIsEmpty(mqs[index].file);
}

int8_t MQisFull(uint64_t index) {
	if(index < 0 || index >= MAX_MQS || mqs[index].file == NULL) {
		return -1;
	}
	return basicFileIsFull(mqs[index].file);
}

void printMQs(void) {
	ncPrint("\n------ Message Queues ------");
	ncPrint("\nname             reader PID    writer PID    size(B)    free space(B)\n");
	for(uint64_t i = 0, printedMQs = 0; i < MAX_MQS && printedMQs < numMQs; i++) {
		if(mqs[i].file != NULL) {
			//Name
			char *name = getBasicFileName(mqs[i].file);
			ncPrint(name);
			pad(MAX_NAME + 1 - strlen(name));
			//Reader PID
			char buff[22];
			if(mqs[i].readPID == -1) {
				buff[0] = '-';
				buff[1] = 0;
			}
			else {
				intToStr(mqs[i].readPID, buff);
			}
			ncPrint(buff);
			pad(14-strlen(buff));
			//Writer PID
			if(mqs[i].writePID == -1) {
				buff[0] = '-';
				buff[1] = 0;
			}
			else {
				intToStr(mqs[i].writePID, buff);
			}
			ncPrint(buff);
			pad(14-strlen(buff));
			//Size
			intToStr(getBasicFileSize(mqs[i].file), buff);
			ncPrint(buff);
			pad(11-strlen(buff));
			//Free space
			intToStr(getBasicFileFreeSpace(mqs[i].file), buff);
			ncPrint(buff);
			ncPrintChar('\n');
			printedMQs++;
		}
	}
	ncPrintChar('\n');
}


/* **********************************************
*				Static functions
* **********************************************/
static int64_t indexOfMQ(const char* name) {
	for(uint64_t i = 0, checkedMQs = 0; checkedMQs < numMQs && i < MAX_MQS; i++) {
		if(mqs[i].file != NULL) {
			if(streql(getBasicFileName(mqs[i].file), name)) {
				return i;
			}
			checkedMQs++;
		}
	}
	return -1;
}

static int8_t newMQ(const char* name, uint64_t tableIndex, uint32_t accessFlags) {
	//TODO verify flags with &, not & and implement NOBLOCK flag
	if(mqs[tableIndex].file != NULL || ((accessFlags & F_READ) == 0 && (accessFlags & F_WRITE) == 0)) {
		return -1;
	}
	BasicFile file = createBasicFileWithName(name);
	if(file == NULL) {
		return -1;
	}
	//Actually create MQ struct
	uint64_t pid = getCurrentPID();
	mqs[tableIndex] = (MessageQueue) {
		file,
		0,		//Not setting links, openMQ increments them
		-1,		//Not setting access PIDs here, delegating to markAccess() below
		-1
	};
	if(markAccess(tableIndex, pid, accessFlags) == -1) {
		destroyBasicFile(file);
		mqs[tableIndex].file = NULL;
		return -1;
	}
	numMQs++;
	return 1;
}

static int8_t markAccess(uint64_t tableIndex, uint64_t pid, uint32_t accessFlags) {
	if(mqs[tableIndex].file == NULL) {
		return -1;
	}
	if(accessFlags & F_READ) {
		if(mqs[tableIndex].readPID != -1) {
			return -1;
		}
		mqs[tableIndex].readPID = pid;
		return 1;
	}
	else if(accessFlags & F_WRITE) {
		if(mqs[tableIndex].writePID != -1) {
			return -1;
		}
		mqs[tableIndex].writePID = pid;
		return 1;
	}
	else {
		return -1;
	}
}

static int8_t destroyMQ(uint64_t index) {
	BasicFile f = mqs[index].file;
	if(f == NULL) {
		return 0;
	}
	destroyBasicFile(f);
	mqs[index].file = NULL;	//TODO consider clearing out the whole struct rather than just the File?
	numMQs--;
	return 1;
}

static int64_t findFreeSlot() {
	for(uint64_t i = 0; i < MAX_MQS; i++) {
		if(mqs[i].file == NULL) {
			return (int64_t) i;
		}
	}
	return -1;
}