#include <lock.h>

void lock(uint8_t* lockVariable) {
	while(!tsl(lockVariable)) {
		// yield();
	}
}

void unlock(uint8_t* lockVariable) {
	//TODO make atomic?
	*lockVariable = 0;
}