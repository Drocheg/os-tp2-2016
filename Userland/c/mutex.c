#include <mutex.h>
#include <syscalls.h>

void mutex_lock(uint8_t *mutex) {
	_int80(MUTEX_LOCK, (uint64_t)mutex);
}

void mutex_unlock(uint8_t *mutex) {
	_int80(MUTEX_UNLOCK, (uint64_t)mutex);
}