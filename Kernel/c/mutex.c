#include <mutex.h>
#include <scheduler.h>

void mutex_lock(uint8_t *mutex) {
	while(tsl(mutex) == 1) {
		yield();
	}
}

void mutex_unlock(uint8_t *mutex) {
	*mutex = 0;
}