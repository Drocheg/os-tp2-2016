#ifndef MUTEX_H
#define MUTEX_H

#include <stdint.h>

/**
* Blocks the calling process until a mutex can be set.
*
* @param mutex Memory address of a byte to attempt to obtain a lock on.
*/
void mutex_lock(uint8_t *mutex);

/**
* Unlocks a previously set mutex.
*
* @param mutex Memory address of mutex to unlock.
*/
void mutex_unlock(uint8_t *mutex);

#endif