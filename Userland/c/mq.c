#ifndef MQ_H
#define MQ_H

#include <mq.h>
#include <interrupts.h>
#include <syscalls.h>
#include <stddef.h>
#include <usrlib.h>

static int8_t MQreceiveChar(uint64_t descriptor, char *dest);
static int8_t MQsendChar(uint64_t descriptor, char *src);

int64_t MQopen(const char* name, uint32_t accessFlags) {
	int64_t result;
	_int80(MQ_OPEN, (uint64_t) name, (uint64_t) accessFlags, (uint64_t)&result);
	return result;
}

int8_t MQclose(uint64_t descriptor) {
	int64_t result;
	_int80(MQ_OPEN, descriptor, (uint64_t)&result, 0);
	return result;
}

int64_t MQreceive(uint64_t descriptor, char *buff, size_t buffLen) {
	int64_t result;
	for(result = 0; result < buffLen; result++) {
		if(MQreceiveChar(descriptor, buff+result) == -1) {
			return result == 0 ? -1 : result;
		}
	}
	return result;
}

/**
* Receives a single character from the specified message queue, storing it in dest.
*
* @return The number of characters read (1 or 0 on EOF), or -1 on error.
*/
static int8_t MQreceiveChar(uint64_t descriptor, char *dest) {
	int8_t result;
	_int80(MQ_RECEIVE, descriptor, (uint64_t) dest, (uint64_t) &result);
	return result;
}

int64_t MQsend(uint64_t descriptor, const char *msg, size_t msgLen) {
	int64_t result;
	for(result = 0; result < msgLen; result++) {
		if(MQsendChar(descriptor, &msg[result]) == -1) {
			return result == 0 ? -1 : result;
		}
	}
	return result;
}

/**
* Sends a single character to the specified message queue, reading it from dest.
*
* @return The number of characters sent (1 or 0 on EOF), or -1 on error.
*/
static int8_t MQsendChar(uint64_t descriptor, char *src) {
	int8_t result;
	print("\nSending '");
	char la[2] = {*src, 0};
	print(la);
	print("' via MQ with descriptor ");
	printNum(descriptor);
	print("...\n");
	_int80(MQ_SEND, descriptor, (uint64_t) src, (uint64_t) &result);
	print("\nsent, returned ");
	printNum(result);
	return result;
}

#endif