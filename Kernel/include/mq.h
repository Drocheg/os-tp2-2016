#ifndef MQ_H
#define MQ_H

#include <stddef.h>
#include <stdint.h>
#include <file.h>

/**
* Opens a message queue in the specified access mode (either F_READ or F_WRITE).
* NOTE: Only 1 process may have a message queue open in a particular access mode at a time.
* 
* @return A message queue descriptor for the solicited message queue, or NULL on error.
*/
uint64_t MQopen(const char* name, uint8_t accessMode);

/**
* Closes the message queue with the given descriptor.
* NOTE: The queue itself will not be erased until all processes who have MQopen()ed it call MQclose() on it.
*
* @return 0 if the MQ was unlinked successfully but not destroyed, 1 if unlinked successfully and destroyed,
* -1 on error.
*/
int8_t MQclose(uint64_t descriptor);

/**
* Receives a message from the specified message queue, of up to buffLen bytes, writing it to buff
* NOTE: Will block the process if there is nothing to read.
*
* @return The actual number of bytes received, or -1 on error.
*/
int8_t MQreceive(uint64_t descriptor, char *buff, size_t buffLen);

/**
* Receives a message from the specified message queue, of up to buffLen bytes, writing it to buff
* If there is no more data available, returns.
*
* @return The actual number of bytes received, or -1 on error.
*/
int8_t MQreceiveNoblock(uint64_t descriptor, char *buff, size_t buffLen);

/**
* Sends the specified message (of length mgsLen) to the specified message queue.
* NOTE: Will block the process if the message queue is full.
*
* @return The actual number of bytes sent, or -1 on error.
*/
int8_t MQsend(uint64_t descriptor, const char *msg, size_t mgsLen);

/**
* Sends the specified message (of length mgsLen) to the specified message queue.
* Will fail if the message queue is full.
*
* @return The actual number of bytes sent, or -1 on error.
*/
int8_t MQsendNoblock(uint64_t descriptor, const char *msg, size_t mgsLen);

#endif