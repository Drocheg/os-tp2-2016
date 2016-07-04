#ifndef MQ_H
#define MQ_H

#include <stddef.h>
#include <stdint.h>
#include <file.h>

/**
* Opens a message queue with the specified name, with the specified access parameters.
*
* @return A file descriptor for the calling process to reference the opened MQ,
* or -1 on error.
*/
int64_t MQopen(const char* name, uint32_t accessFlags);

/**
* Stores a unique MQ name in the specified destination buffer. The name can be used to
* create a new MQ.
*
* @param dest Where to store the name. WARNING: Make sure dest is of size MAX_NAME+1.
* @return 0 if a name was successfully stored, -1 on error.
*/
int8_t MQuniq(char *dest);

/**
* Reads a single character from the specified message queue, storing it in dest.
*
* @param index Index of the MQ in the MQ table. Not a file descriptor.
* @param dest Where to store the read char.
* @return The number of read characters (1 or 0 in case of EOF), or -1 on invalid parameters.
*/
int8_t MQreadChar(uint64_t index, char *dest);

/**
* Writes a single character to the specified message queue, reading it from dest.
*
* @param index Index of the MQ in the MQ table. Not a file descriptor.
* @param dest Where to get the character to write.
* @return The number of written characters (1 or 0 in case of EOF), or -1 on invalid parameters.
*/
int8_t MQwriteChar(uint64_t index, char *src);

/**
* Checks whether the message queue at the specified index is empty.
*
* @param index Index of the MQ in the MQ table. Not a file descriptor.
* @return 1 if empty, 0 otherwise, -1 on error (e.g. nonexistent MQ).
*/
int8_t MQisEmpty(uint64_t index);

/**
* Checks whether the message queue at the specified index is full.
*
* @param index Index of the MQ in the MQ table. Not a file descriptor.
* @return 1 if full, 0 otherwise, -1 on error (e.g. nonexistent MQ).
*/
int8_t MQisFull(uint64_t index);

/**
* Closes the message queue with the given descriptor.
* NOTE: The queue itself will not be destroyed until all processes who have MQopen()ed it call MQclose() on it.
*
* @param index Index of the MQ in the MQ table. Not a file descriptor.
* @return 0 if the MQ was unlinked successfully but not destroyed, 1 if unlinked successfully and destroyed,
* -1 on error.
*/
int8_t MQclose(uint64_t index);

/**
* Prints a formatted list of currently opened message queues.
*/
void printMQs(void);

#endif