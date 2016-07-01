#ifndef PROCESS_H
#define PROCESS_H

// #include <file.h>
#include <fileManager.h>
#include <stdint.h>
#include <processCommon.h>


#define MAX_FILES 16


typedef enum {INPUT = 0, OUTPUT} IOOperation;


/*
 * Creates a PCB
 * Returns 0 on success, or -1 otherwise
 */
uint64_t initializePCB();

/*
 * Creates a process and loads it into the PCB
 * Returns process PCB index on success.
 * Returns -1 if PCB wasn't initialized
 * Returns -2 if no space in PCB.
 * Returns -3 if no memory for the stack.
 * Returns -4 if there was a problem with parameters
 */
uint64_t createProcess(uint64_t parentPid, char name[MAX_NAME_LENGTH], void *entryPoint, uint64_t argc, char *argv[]);


/* **************************************
*				Getters
* **************************************/

/**
* @return The ID of the currently running process.
*/
uint64_t getCurrentPID();

uint64_t getProcessPID(uint64_t PCBIndex);

uint64_t getProcessParentPID(uint64_t PCBIndex);

char *getProcessName(uint64_t PCBIndex);

void *getProcessStack(uint64_t PCBIndex);

uint64_t getFilesQuantity(uint64_t PCBIndex);

uint32_t getFileFlags(uint64_t PCBIndex, uint64_t fileDescriptor);

/* Setters */
uint64_t setProcessStack(uint64_t PCBIndex, void *stack);

int64_t setFileFlags(uint64_t PCBIndex, uint64_t fileDescriptor, uint32_t flags);



/* **************************************
*			File Management
* **************************************/

/*
 * Adds a file to the process' files table.
 * Returns the file's descriptor on sucess, or -1 otherwise.
 */
uint64_t addFile(uint64_t PCBIndex, uint32_t index, FileType fileType, uint32_t flags);

/*
 * Removes a file from the process' files table.
 * If no file in <fileDescriptor> entry, nothing happens.
 * Returns 0 on sucess, or -1 otherwise.
 */
uint64_t removeFile(uint64_t PCBIndex, uint64_t fileDescriptor);

/*
 * Returns 1 if there is a file in the <fileDescriptor> position of process' file table, or 0 otherwise
 */
uint64_t existsFile(uint64_t PCBIndex, uint64_t fileDescriptor);

/*
 * Generic function to operate over a file.
 * <PCBIndex> states which process will operate over the file pointed by <fileDescriptor>.
 * Four operation can be done (READ, WRITE, ASK IF THERE IS DATA AVAILABLE, OR ASK IF THERE IS SPACE TO WRITE)
 * For more info. see <fileManager.h>
 * The last parameter (<character>) is used for READ and WRITE operations. When reading, the read byte
 * will be written into the position stated by this pointer. When writing, the written byte will be
 * that one pointed by <character>.
 * Note: JUST ONE BYTE WILL BE READ OR WRITTEN
 *
 * Returns 0 on success, or -1 otherwise. 
 * Note: for IS_EMPTY and IS_FULL, 0 is returned when true, and -1 when false
 */
int64_t operateFile(uint64_t PCBIndex, uint64_t fileDescriptor, FileOperation operation, char *character);

/*
 * Takes the process with <PCBIndex> from the table
 * Returns 0 on success, or -1 otherwise
 */
uint64_t destroyProcess(uint64_t PCBIndex);



void *malloc(uint64_t PCBIndex, int64_t size);






#endif /* PROCESS_H */