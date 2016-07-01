#ifndef FILE_H
#define FILE_H

/*
* This kernel manages a series of different "files." A File is an abstraction
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
* identified by its unique name, of limited length. The FileManager is responsible
* for interfacing with the different types of Files available.
*/

/*
* Bascic File operations, as mentioned bove.
*/
typedef enum {OPEN = 0, READ, WRITE, IS_EMPTY, IS_FULL, CLOSE} FileOperation;


/*
* End Of file - indicates that a File cannot be read from or written to, or
* that there was an error carrying out an operation.
*/
#define EOF -1
/*
* Maximum length of a File name
*/
#define MAX_NAME 16

#endif /* FILE_H */
