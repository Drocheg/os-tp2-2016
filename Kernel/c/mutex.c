#include <mutex.h>
#include <scheduler.h>

void mutex_lock(volatile uint8_t *mutex) {
	while(tsl(mutex) == 1) {
		yield();
	}
}

void mutex_unlock(volatile uint8_t *mutex) {
	*mutex = 0;
}
