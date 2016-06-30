#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H


/*
 * Each file type has an entry in the file manager table
 */
typedef enum {STDIN_ = 0, STDOUT_, STDERR_, MESSAGE_QUEUES} FileType;
typedef enum {READ = 0, WRITE, AVAILABLE_DATA, FREE_SPACE} FileOperation;

// /*
//  * We assume that every "file" is saved into a table (each type in it's table).
//  * If a file is unique (for example, video memory or keyboard buffer),
//  * a wrapper function must be implemented.
//  */
// typedef uint64_t (*ReadCharFn)(uint64_t index);
// typedef uint64_t (*WriteCharFn)(char c, uint64_t index);
// typedef uint64_t (*DataAvailableFn)(uint64_t index);
// typedef uint64_t (*HasFreeSpaceFn)(uint64_t index);



/*
 * Initializes the file manager
 */
void initializeFileManager();

/*
 * Make <operation> over <fileType> <index> file. Parameter <c> is used when operation is WRITE
 * Returns non-negative integer on success, or -1 otherwise.
 * 
 * Note: for AVAILABLE_DATA and FREE_SPACE, 0 is returned when true, and -1 when false
 */
uint64_t operate(FileOperation operation, FileType fileType, int64_t fileIndex, char c);




#endif /* FILE_MANAGER_H */