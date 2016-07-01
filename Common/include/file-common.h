#ifndef FILE_COMMON_H
#define FILE_COMMON_H

/*
* This system manages a series of different "files." A File is an abstraction
* over any readable or writeable section of data (similar to Java's Stream
* abstraction) and supports the following operations:
* - Create
* - Destroy
* - Read
* - Write
* - Is full
* - Is empty
*
* Each File implements its own behavior for these operations. Each File is
* identified by its unique name, of limited length.
*/

/*
* End Of file - indicates that a File cannot be read from or written to, or
* that there was an error carrying out an operation.
*/
#define EOF -1
/*
* Maximum length of a File name
*/
#define MAX_NAME 16

/*
* File creation/access flags
*/
#define F_READ 0x01
#define F_WRITE 0x02
#define F_NOBLOCK 0x04

#endif