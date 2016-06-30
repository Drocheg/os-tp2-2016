#include <mq.h>
#include <file.h>
#include <kernel-lib.h>
#include <stdlib.h>

#define MAX_MQS 256

typedef struct {
	const char* name;		//Name through which processes can identify this message queue.
	uint64_t links;			//The number of processes who have this message queue open. Upon reaching 0, it is destroyed.
	File file;				//File where actual data is read/written
	uint8_t openForRead;	//Whether this MQ has been opened for reading
	uint8_t openForWrite;	//Whether this MQ has been opened for writing
} MessageQueue;

static MessageQueue[MAX_MQS] mqs = {NULL};		//Initialize with NULL
static uint64_t numMQs = 0;

/**
* @return The index of the message queue with the specified name, or -1 if not found.
*/
int64_t indexOfMQ(const char* name);

/**
* Creates a new message queue with the given name.
* @return The index at which the MQ was created, or -1 on error.
*/
int64_t newMQ(const char* name);

/*
* Marks the specified message queue as open in the specified mode.
* @return 1 on success, -1 on error (i.e. already open in the specified mode)
*/
int8_t markAccess(MessageQueue mq, uint8_t accessMode);

/*
* Destroys the message queue and its resources at the specified index, if it exists.
*/
void destroyMQ(uint64_t index);

mqd_t MQopen(const char* name, uint8_t accessMode); {
	if(name == NULL || (accessMode != F_READ && accessMode != F_WRITE)) {
		return NULL;
	}
	int64_t index = indexOfMQ(name);
	//Nonexistant MQ, create
	if(index == -1) {
		index = newMQ(name);
		if(index == -1) {
			return NULL;
		}
		if(markAccess(mqs[index], accessMode) == -1) {	//Set as opened in the specified access mode
			return NULL;
		}
		mqd_t result = malloc(sizeof(*result));	//TODO malloc
		result->tableIndex = index;
		result->accessMode = accessMode;
		return result;
	}
	//Add link to existing MQ
	else {
		//Only 1 process can have the MQ open for reading/writing at the same time
		if(markAccess(mqs[index], accessMode) == -1) {
			return NULL;
		}
		mqd_t result = malloc(sizeof(*result));	//TODO malloc
		result->tableIndex = index;
		result->accessMode = accessMode;
		return result;
	}
}

int8_t MQclose(mqd_t descriptor) {
	uint64_t index = descriptor.tableIndex;
	MessageQueue mq = mqs[index];
	if(mq == NULL) {
		return -1;
	}
	if(mq.accessMode != descriptor.accessMode) {	//Shouldn't happen
		return -1;
	}
	//TODO free() the descriptor? Otherwise users could still access the MQ
	mq.links--;
	if(mq.links == 0) {		//No more links, destroy
		destroyMQ(descriptor.tableIndex);
		return 1;
	}
	else {
		return 0;
	}
}

int8_t MQreceive(mqd_t descriptor, char *buff, size_t buffLen) {
	if(descriptor.accessMode != F_READ || mqs[descriptor.tableIndex] == NULL) {
		return -1;
	}
	File f = mqs[descriptor.tableIndex].file;
	size_t readBytes = 0;
	for(readBytes = 0; readBytes < buffLen; readBytes++) {
		int8_t c = readChar(f);
		buff[readBytes] = c;
		if(c == EOF) {
			break;
		}
	}
	return readBytes;
}

int8_t MQreceiveNoblock(mqd_t descriptor, char *buff, size_t buffLen) {
	if(descriptor.accessMode != F_READ || mqs[descriptor.tableIndex] == NULL) {
		return -1;
	}
	File f = mqs[descriptor.tableIndex].file;
	size_t readBytes = 0;
	for(readBytes = 0; readBytes < buffLen; readBytes++) {
		if(!dataAvailable(f)) {
			break;
		}
		int8_t c = readChar(f);
		buff[readBytes] = c;
		if(c == EOF) {
			break;
		}
	}
	return readBytes;
}

int8_t MQsend(mqd_t descriptor, const char *msg, size_t mgsLen) {
	if(descriptor.accessMode != F_WRITE || mqs[descriptor.tableIndex] == NULL) {
		return -1;
	}
	File f = mqs[descriptor.tableIndex].file;
	size_t writtenBytes = 0;
	for(writtenBytes = 0; writtenBytes < buffLen; writtenBytes++) {
		writeChar(msg[writtenBytes], f);
	}
	return writtenBytes;
}

int8_t MQsendNoblock(mqd_t descriptor, const char *msg, size_t mgsLen) {
	if(descriptor.accessMode != F_WRITE || mqs[descriptor.tableIndex] == NULL) {
		return -1;
	}
	File f = mqs[descriptor.tableIndex].file;
	size_t writtenBytes = 0;
	for(writtenBytes = 0; writtenBytes < buffLen; writtenBytes++) {
		if(!hasFreeSpace(f)) {
			break;
		}
		writeChar(msg[writtenBytes], f);
	}
	return writtenBytes;
}

int64_t indexOfMQ(const char* name) {
	for(uint64_t i = 0; i <  numMQs; i++) {
		if(mqs[i] != NULL && strcmp(mqs[i].name, name)) {
			return i;
		}
	}
	return -1;
}

int64_t newMQ(const char* name) {
	for(uint64_t i = 0; i <  numMQs; i++) {
		//TODO memcpy() the name, needs malloc
		if(mqs[i] == NULL) {
			File file = createFileWithName(name);
			if(file == NULL) {
				return -1;
			}
			//Create the fresh MQ with 1 link
			mqs[i] = {
				name,
				1,
				file
			};
			numMQs++;
			return (int64_t) i;
		}
	}
	//No more room
	return -1;
}

int8_t markAccess(MessageQueue mq, uint8_t accessMode) {
	if(mq == NULL) {
		return -1;
	}
	if(accessMode == F_READ) {
		if(mq.openForRead) {
			return -1;
		}
		mq.openForRead = 1;
		return 1;
	}
	else {
		if(mq.openForWrite) {
			return -1;
		}
		mq.openForWrite = 1;
		return 1;	
	}
}

void destroyMQ(uint64_t index) {
	MessageQueue mq = mqs[index];
	if(mq == NULL) {
		return;
	}
	destroyFile(mq.file);
	//TODO free() the name?
	mqs[index] = NULL;
	numMQs--;
}