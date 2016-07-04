#include <libasm.h>
#include <syscalls.h>


void read(uint8_t fd, char *buff, uint64_t maxBytes) {
	_int80(SYSREAD, fd, (uint64_t)buff, maxBytes);
}

void write(uint8_t fd, char *buff, uint64_t maxBytes) {
	_int80(SYSWRITE, fd, (uint64_t)buff, maxBytes);
}




void reboot_sys() {
	_int80(REBOOT, 0, 0, 0);
}


void scroll_sys() {
	_int80(SCROLL, 1, 0, 0);
}


void sleep_sys(uint64_t miliseconds) {
	_int80(SLEEP, miliseconds, 0, 0);
}