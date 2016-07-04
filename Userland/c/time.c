#include <libasm.h>
#include <syscalls.h>

uint64_t time() {
	uint64_t result;
	_int80(TIME, (uint64_t)&result, 0, 0);
	return result;
}