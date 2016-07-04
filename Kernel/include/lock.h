#ifndef LOCK_H
#define LOCK_H

#include <stdint.h>

/*
* Atomically attempts to set *lockVariable to 1, if not already set.
* If *lockVariable == 0, it is atomically set to 1. Otherwise it's left
* as it was.
*
* @param lockVariable Memory address where to test-and-set for a 0 or a 1.
* @return 1 If the lock was set, 0 otherwise.
*/
uint8_t tsl(uint8_t* lockVariable);

void lock(uint8_t* lockVariable);

void unlock(uint8_t* lockVariable);


#endif