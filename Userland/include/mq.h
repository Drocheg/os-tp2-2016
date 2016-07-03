#ifndef MQ_H
#define MQ_H

#include <stdint.h>
#include <stddef.h>

/**
* Opens a message queue in the specified access mode (either F_READ or F_WRITE).
* NOTE: Only 1 process may have a message queue open in a particular access mode at a time.
* 
* @param name The name that identifies this message queue. Max length is MAX_FILE.
* @return A message queue descriptor for the solicited message queue, or -1 on error.
*/
int64_t MQopen(const char* name, uint32_t accessFlags);

/**
* Closes the message queue with the given descriptor.
* NOTE: The queue itself will not be erased until all processes who have MQopen()ed it call MQclose() on it.
*
*-----------------------------------------------------------------------------------------------------------
* @return 0 if the MQ was unlinked successfully but not destroyed, 1 if unlinked successfully and destroyed,
* -1 on error.
*				^^ NO, Kernel functions are limited to returning 0 on success and -1 on error
*				\/ Use this for now (-1 will always be error and >= 0 will be success)
*-----------------------------------------------------------------------------------------------------------
* @return 1 on success, -1 on error.
*/
int8_t MQclose(uint64_t descriptor);

/**
* Checks whether the specified message queue is full. If full, blocking writes will block and non-blocking
* writes will write 0 characters.
*
* @return 1 if the MQ is full, 0 otherwise.
*/
int8_t MQisFull(uint64_t descriptor);

/**
* Checks whether the specified message queue is empty. If empty, blocking reads will block and non-blocking
* reads will read 0 characters.
*
* @return 1 if the MQ is empty, 0 otherwise.
*/
int8_t MQisEmpty(uint64_t descriptor);

/**
* Receives a message from the specified message queue, of up to buffLen bytes, writing it to buff.
* If the MQ was not opened non-blockingly and there is no data to read, the process will block until
* there is enough data to fill buff. If it was opened non-blockingly, the function will return as soon
* as data runs out or when buff is filled, whichever comes first.
* NOTE: Does NOT add a null terminator.
*
* @param descriptor 
*/
int64_t MQreceive(uint64_t descriptor, char *buff, size_t buffLen);

/**
* Receives a single character from the specified message queue, storing it in dest.
* If the MQ was not opened non-blockingly and the MQ is empty, the process will block until the char
* can be read from the MQ.
*
* @return The number of characters received (1 or 0 on EOF in non-block mode), or -1 on error.
*/
int8_t MQreceiveChar(uint64_t descriptor, char *dest);

/**
* Sends the specified message (of length msgLen) to the specified message queue.
* If the MQ was not opened non-blockingly and the MQ is full, the process will block until all of msg
* can be written into the MQ. If it was opened non-blockingly, the function will return as soon as
* all of msg is written or when the MQ fills up, whichever comes first.
* NOTE: Does NOT add a null terminator.
*
* @return The actual number of bytes sent, or -1 on error.
*/
int64_t MQsend(uint64_t descriptor, char *msg, size_t mgsLen);

/**
* Sends a single character to the specified message queue, reading it from src.
* If the MQ was not opened non-blockingly and the MQ is full, the process will block until the char
* can be written into the MQ.
*
* @return The number of characters sent (1 or 0 on EOF in non-block mode), or -1 on error.
*/
int8_t MQsendChar(uint64_t descriptor, char *src);


#endif