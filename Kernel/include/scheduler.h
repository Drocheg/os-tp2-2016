#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>


/*
 * Sets up the scheduler 
 * Must call initializePageStack!!
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
int64_t addProcess(uint64_t parentPid, char name[MAX_NAME_LENGTH], void *entryPoint, uint64_t argc, char *argv[]);

/*
 * Updates the process queue, changing to the nexr process
 * Returns the next process' stack, or NULL if no process is scheduled
 */
void *nextProcess();

/*
 * Finishes the current process
 * Returns 0 on suceess, or -1 otherwise
 */
uint64_t finishProcess();


void printPS();





#endif /* SCHEDULER_H */