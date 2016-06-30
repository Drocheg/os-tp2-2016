#include <kernel-lib.h>
#include <fileDescriptors.h>
#include <keyboard.h>
#include <libasm.h>
#include <scanCodes.h>
#include <modules.h>
#include <speaker.h>
#include <video.h>
#include <scheduler.h>
#include <fileManager.h>
#include <process.h>
#include <interrupts.h>


uint64_t read(uint64_t fd, char *buffer, uint64_t maxBytes) {

	return fileOperation(fd, buffer, maxBytes, INPUT, 1);
}



uint64_t write(uint64_t fd, char **buffer, uint64_t maxBytes) {

	return fileOperation(fd, buffer, maxBytes, OUTPUT, 1);
}


//Reads from different defined files: processed keyboard and unprocessed keyboard
int64_t sys_read(uint8_t fd, char *buff, uint32_t maxBytes) {
	int64_t result;
	if(fd < MIN_FD || fd > MAX_FD) return -1;
	int i, done;

	switch(fd) {
		case STDIN:
			result = 0;
	  		done = 0;
	  		do {
				for(i = 0; !bufferIsEmpty() && i < maxBytes && !done; i++) {
					buff[i]= pollProcessedKey();
				  	if(buff[i] == '\n') {
				  		done = 1;
				  	}
					result++;
				}
				_halt();
			}while(!done && result < maxBytes);
	  		break;
  		case STDIN_RAW:
  			result = 0;
	  		done = 0;
	  		do {
				for(i = 0; !bufferIsEmpty() && i < maxBytes && !done; i++) {
					buff[i]= pollRawKey();
				  	if(decodeScanCode(buff[i]) == '\n') {
				  		done = 1;
				  	}
					result++;
				}
				_halt();
			}while(!done && result < maxBytes);
  			break;
		default:
			result = -1;
  			break;
	}
	return result;
}

//Writes to different defined files: screen, error stream
int64_t sys_write(uint8_t fd, char *buff, uint32_t maxBytes) {
	int64_t result;
	if(fd < MIN_FD || fd > MAX_FD) return -1;
	int i;
	switch(fd) {
		case STDOUT:
			result = 0;
			i = 0;
			while(*buff != 0 && i < maxBytes) {
				ncPrintChar(*buff);
				buff++;
				result++;
			}
			break;
		case STDERR:
			result = 0;
			i = 0;
			while(*buff != 0 && i < maxBytes) {
				ncPrintColorChar(*buff, (char)0x4F);
				buff++;
				result++;
			}
			break;
		default:
			result = -1;
			break;
	}
	return result;
}

//Writes notes and frequencies to the pc speaker
void sys_sound(uint32_t freq, uint32_t time) {
	offerSound(freq, time);
}

void * memset(void * destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

void *memcpy(void *destination, const void *source, uint64_t length)
{
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i = 0;

	if ((uint64_t)destination % sizeof(uint32_t) == 0
		&& (uint64_t)source % sizeof(uint32_t) == 0
		&& length % sizeof(uint32_t) == 0) {

		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++) {
			d[i] = s[i];
		}
	} else {
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++) {
			d[i] = s[i];
		}
	}

	return destination;
}






