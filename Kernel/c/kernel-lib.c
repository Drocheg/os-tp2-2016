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


//Reads from different defined files: processed keyboard and unprocessed keyboard

// int64_t sys_read(uint8_t fd, char *buff, uint32_t maxBytes) {
// 	int64_t result;
// 	if(fd < MIN_FD || fd > MAX_FD) return -1;
// 	int i, done;

// 	switch(fd) {
// 		case STDIN:
// 			result = 0;
// 	  		done = 0;
// 	  		do {
// 				for(i = 0; !bufferIsEmpty() && i < maxBytes && !done; i++) {
// 					buff[i]= pollProcessedKey();
// 				  	if(buff[i] == '\n') {
// 				  		done = 1;
// 				  	}
// 					result++;
// 				}
// 				yield();
// 			}while(!done && result < maxBytes);
// 	  		break;
//   		case STDIN_RAW:
//   			result = 0;
// 	  		done = 0;
// 	  		do {
// 				for(i = 0; !bufferIsEmpty() && i < maxBytes && !done; i++) {
// 					buff[i]= pollRawKey();
// 				  	if(decodeScanCode(buff[i]) == '\n') {
// 				  		done = 1;
// 				  	}
// 					result++;
// 				}
// 				yield();
// 			}while(!done && result < maxBytes);
//   			break;
// 		default:
// 			result = -1;
//   			break;
// 	}
// 	return result;
// }

// //Writes to different defined files: screen, error stream
// int64_t sys_write(uint8_t fd, char *buff, uint32_t maxBytes) {
// 	int64_t result;
// 	if(fd < MIN_FD || fd > MAX_FD) return -1;
// 	int i;
// 	switch(fd) {
// 		case STDOUT:
// 			result = 0;
// 			i = 0;
// 			while(*buff != 0 && i < maxBytes) {
// 				ncPrintChar(*buff);
// 				buff++;
// 				result++;
// 			}
// 			break;
// 		case STDERR:
// 			result = 0;
// 			i = 0;
// 			while(*buff != 0 && i < maxBytes) {
// 				ncPrintColorChar(*buff, (char)0x4F);
// 				buff++;
// 				result++;
// 			}
// 			break;
// 		default:
// 			result = -1;
// 			break;
// 	}
// 	return result;
// }

//Writes notes and frequencies to the pc speaker
void sys_sound(uint32_t freq, uint32_t time) {
	playSound(freq);
}








void printIPCS(){
	ncPrint("IPCS");
}
