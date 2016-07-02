#include <mq.h>
#include <file.h>
#include <basicFile.h>
#include <kernel-lib.h>
#include <stdlib.h>
#include <process.h>
#include <scheduler.h>
#include <fileManager.h>

#define MAX_MQS 256

typedef struct {
	BasicFile file;			//File where actual data is read/written
	uint64_t links;			//The number of processes who have this message queue open. Upon reaching 0, it is destroyed.
	int64_t readPID;		//ID of the process who has this MQ open for reading. -1 if none.
	int64_t writePID;		//ID of the process who has this MQ open for writing. -1 if none.
} MessageQueue;

static MessageQueue mqs[MAX_MQS] = {NULL};		//Initialize with NULL
static uint64_t numMQs = 0;

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
*			FileManager imlpementations
* **********************************************/
int64_t MQopen(const char* name, uint32_t accessFlags) {
	if(name == NULL || ((accessFlags & F_READ) == 0 && (accessFlags & F_WRITE) == 0)) {
		return -1;
	}
	int64_t tableIndex = indexOfMQ(name);
	//Nonexistant MQ, create
	if(tableIndex == -1) {
		tableIndex = findFreeSlot();
		if(tableIndex == -1) {
			return -1;
		}
		if(newMQ(name, tableIndex, accessFlags) == -1) {
			return -1;
		}
	}
	//Existint MQ, check priviledges
	else {
		//Only 1 process can have the MQ open for reading/writing at the same time
		if(markAccess(tableIndex, getCurrentPID(), accessFlags) == -1) {
			return -1;
		}
	}
	//MQ valid, register it in the current process' PCB
	int64_t MQdescriptor = addFile(getCurrentPCBIndex(), tableIndex, MESSAGE_QUEUE, accessFlags);
	if(MQdescriptor == -1) {
		return -1;
	}
	mqs[tableIndex].links++;
	return (int64_t) MQdescriptor;
}

int8_t MQclose(uint64_t tableIndex) {
	MessageQueue mq = mqs[tableIndex];
	if(mq.file == NULL) {
		return -1;
	}
	uint64_t pid = getCurrentPID();
	if(mq.readPID == pid) {
		mq.readPID = -1;
	}
	else if(mq.writePID == pid) {
		mq.writePID = -1;
	}
	else {
		return -1;
	}
	
	mq.links--;
	if(mq.links == 0) {		//No more links, destroy
		return destroyMQ(tableIndex) ? 1 : -1;
	}
	else {
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


/* **********************************************
*				Static functions
* **********************************************/
static int64_t indexOfMQ(const char* name) {
	for(uint64_t i = 0; i <  MAX_MQS; i++) {
		if(mqs[i].file != NULL && streql(getBasicFileName(mqs[i].file), name)) {
			return i;
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
		1,
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