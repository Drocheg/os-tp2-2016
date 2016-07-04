#ifndef MQ_H
#define MQ_H

#include <mq.h>
#include <interrupts.h>
#include <syscalls.h>
#include <stddef.h>
#include <usrlib.h>
#include <libasm.h>

int64_t MQopen(const char* name, uint32_t accessFlags) {
	int64_t result;
	_int80(MQ_OPEN, (uint64_t) name, (uint64_t) accessFlags, (uint64_t)&result);
	return result;
}

int8_t MQclose(uint64_t descriptor) {
	int8_t result;
	_int80(MQ_CLOSE, descriptor, (uint64_t)&result, 0);
	return result;
}

int8_t MQisFull(uint64_t descriptor) {
	int8_t result;
	_int80(MQ_IS_FULL, descriptor, (uint64_t)&result, 0);
	return result;
}

int8_t MQisEmpty(uint64_t descriptor) {
	int8_t result;
	_int80(MQ_IS_EMPTY, descriptor, (uint64_t)&result, 0);
	return result;
}

int8_t MQreceiveChar(uint64_t descriptor, char *dest) {
	int8_t result;
	_int80(MQ_RECEIVE, descriptor, (uint64_t) dest, (uint64_t) &result);
	return result;
}

int64_t MQreceive(uint64_t descriptor, char *buff, size_t buffLen) {
	int64_t result;
	for(result = 0; result < buffLen; result++) {
		int64_t recvResult = MQreceiveChar(descriptor, buff+result);
		if(recvResult == 0) {
			//No bytes received - e.g. readubg with F_NOBLOCK and file is empty
			return result;
		}
		else if(recvResult == -1) {
			//Failed - return how many bytes were received, or error error if none were received.
			return result == 0 ? -1 : result;
		}
	}
	return result;
}

int8_t MQsendChar(uint64_t descriptor, char *src) {
	int8_t result;
	_int80(MQ_SEND, descriptor, (uint64_t) src, (uint64_t) &result);
	return result;
}

int64_t MQsend(uint64_t descriptor, char *msg, size_t msgLen) {
	int64_t result;
	for(result = 0; result < msgLen; result++) {
		int64_t sendResult = MQsendChar(descriptor, &msg[result]);
		if(sendResult == 0) {
			//No bytes sent - e.g. writing with F_NOBLOCK and file is full
			return result;
		}
		else if(sendResult == -1) {
			//Failed - return how many bytes were sent, or error error if none were sent.
			return result == 0 ? -1 : result;
		}
	}
	return result;
}

#endif