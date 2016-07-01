#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <file.h>

/*
* The File manager is responsible for tracking all kinds of File types
* in a centralized place. Since each File reacts differently to its
* basic operations (see file.h), the File manager stores the functions
* to react to each operation in an internal table. Then, for each operation,
* based on the File type, the appropriate function is called.
*/

//Supported File types.
typedef enum {STDIN_ = 0, STDOUT_, STDERR_, MESSAGE_QUEUE} FileType;


/*
* Each File type reacts differently to the operations it supports (see file.h).
* These functions are responsible for responding to different operations. The file
* manager will call the appropriate function based on the File type.
*/

/**
* Reads a single character, storing it in dest.
*
* @param index If the File type has an internal table, the index in said table where
*			   the appropriate File can be found.
* @param dest Where to store the read character.
* @return The number of read characters (1 or 0 in case of EOF), or -1 on invalid parameters.
*/
typedef int8_t (*readCharFn_t)(uint64_t index, char* dest);

/**
* Writes a single character, reading it from dest.
*
* @param index If the File type has an internal table, the index in said table where
*			   the appropriate File can be found.
* @param src Where to read the character to write.
* @return The number of written characters (1 or 0 in case of EOF), or -1 on invalid parameters.
*/
typedef int8_t (*writeCharFn_t)(uint64_t index, char* src);

/**
* Checks whether a File is empty.
*
* @param index If the File type has an internal table, the index in said table where
*			   the appropriate File can be found.
* @return 1 if empty, 0 otherwise, -1 on error.
*/
typedef int8_t (*isEmptyFn_t)(uint64_t index);

/**
* Checks whether a File is full.
*
* @param index If the File type has an internal table, the index in said table where
*			   the appropriate File can be found.
* @return 1 if full, 0 otherwise, -1 on error.
*/
typedef int8_t (*isFullFn_t)(uint64_t index);

/**
* Closes a File. May or may not destroy the File and free up resources.
*
* @param index If the File type has an internal table, the index in said table where
*			   the appropriate File can be found.
* @return A non-negative value on success (0 included), -1 on error.
*/
typedef int8_t (*closeFn_t)(uint64_t index);


/*
 * Initializes the file manager
 */
void initializeFileManager();

/*
 * Make <operation> over <fileType> <index> file. 
 * Parameter <character> is used when operation is READ for storing the result,
 * or WRITE, to store a pointer to the character to be written.
 * It will be used just the first position of the <character> pointer.

 * Returns 0 on success, or -1 otherwise. 
 * Note: for IS_EMPTY and IS_FULL, 0 is returned when true, and -1 when false
 */
uint64_t operate(FileOperation operation, FileType fileType, int64_t fileIndex, char *character);




#endif /* FILE_MANAGER_H */