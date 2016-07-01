#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <processCommon.h>
#include <process.h>

/*
 * Sets up the scheduler 
 * Must call initializePageStack!!
 * Returns 0 on sucess, or -1 otherwise
 */
void initializeScheduler();

/*
 * Starts the scheduler
 */
void startScheduler();

/*
 * Stops the scheduler
 */
void stopScheduler();

/*
 * Adds a process into the queue
 * Returns 0 on success, or -1 otherwise
 */
uint64_t addProcess(uint64_t parentPid, char name[MAX_NAME_LENGTH], void *entryPoint, uint64_t argc, char *argv[]);

/*
 * Makes the scheduler be in charge of an input/output operation of the current process
 * This function will read/write <maxBytes> from/into the file mapped in the process' file table by <fileDescriptor>,
 * into/from <buffer>.
 * It's a blocking function, so it will return when maxBytes are achieved
 * Returns read/written bytes, or -1 if any error ocurred
 */
int64_t fileOperation(uint64_t fileDescriptor, char *buffer, uint64_t maxBytes, IOOperation ioOperation, uint64_t blocking);

/*
 * Stops execution of the running process for <miliseconds> miliseconds.
 * Returns 0 on success (made a <miliseconds> long pause), or -1 otherwise.
 */
uint64_t sleep(uint64_t miliseconds);

/*
 * Updates the process queue, changing to the next process
 * Returns the next process' stack, 
 * or NULL if scheduler is not running or if no process is scheduled
 */
void *nextProcess(void *currentRSP);

/*
 * Returns the current process' PCB index,
 * or -1 if no process sscheduled, or schduler not initialzed
 */
uint64_t getCurrentPCBIndex();

/*
 * Finishes the current process
 * Returns 0 on suceess, or -1 otherwise
 */
uint64_t finishProcess();


void printPS();







#endif /* SCHEDULER_H */