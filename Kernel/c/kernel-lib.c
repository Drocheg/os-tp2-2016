#include <kernel-lib.h>
#include <stdlib.h>
#include <fileDescriptors.h>
#include <keyboard.h>
#include <libasm.h>
#include <scanCodes.h>
#include <modules.h>
#include <speaker.h>
#include <video-common.h>
#include <video.h>
#include <scheduler.h>
#include <fileManager.h>
#include <process.h>
#include <interrupts.h>


int64_t read(uint64_t fd, char *buffer, uint64_t maxBytes) {
	return fileOperation(fd, buffer, maxBytes, INPUT, !(getFileFlags(getCurrentPCBIndex(), fd) & F_NOBLOCK));
}

int64_t write(uint64_t fd, char *buffer, uint64_t maxBytes) {
	return fileOperation(fd, buffer, maxBytes, OUTPUT, !(getFileFlags(getCurrentPCBIndex(), fd) & F_NOBLOCK));
}



//Writes notes and frequencies to the pc speaker
void sys_sound(uint32_t freq, uint32_t time) {
	playSound(freq);
}

