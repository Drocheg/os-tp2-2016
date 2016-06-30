#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H


/*
 * Each file type has an entry in the file manager table
 */
typedef enum {STDIN_ = 0, STDOUT_, STDERR_, MESSAGE_QUEUES} FileType;
typedef enum {READ = 0, WRITE, AVAILABLE_DATA, FREE_SPACE} FileOperation;



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
 * Note: for AVAILABLE_DATA and FREE_SPACE, 0 is returned when true, and -1 when false
 */
uint64_t operate(FileOperation operation, FileType fileType, int64_t fileIndex, char *character);




#endif /* FILE_MANAGER_H */