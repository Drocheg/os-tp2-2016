#ifndef MUTEX_H
#define MUTEX_H

#include <stdint.h>

/**
* Atomically sets *lockVariable to 1, returning its previous value. If this returns
* 0, it means a lock was set.
*
* @param lockVariable Memory address where to test-and-set.
* @return The previous value of *lockVariable. If 0, it means a lock was set.
*/
uint8_t tsl(volatile uint8_t *lockVariable);

/**
* Blocks the calling process until a mutex can be set.
*
* @param mutex Memory address of a byte to attempt to obtain a lock on.
*/
void mutex_lock(volatile uint8_t *mutex);

/**
* Unlocks a previously set mutex.
*
* @param mutex Memory address of mutex to unlock.
*/
void mutex_unlock(volatile uint8_t *mutex);


#endif
