#ifndef SCHEDULER_H
#define SCHEDULER_H


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
 * Updates the process queue, changing to the nexr process
 * Returns the next process' stack, or NULL if no process is scheduled
 */
void *nextProcess();





#endif /* SCHEDULER_H */