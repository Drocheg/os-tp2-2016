#include <usrlib.h>
#include <stdlib.h>
#include <interrupts.h>
#include <syscalls.h>
#include <fileDescriptors.h>
#include <scanCodes.h>
#include <video-common.h>

void fread(uint8_t fd, char *buff, uint64_t maxBytes) {
	_int80(SYSREAD, fd, (uint64_t)buff, maxBytes);
}

void fwrite(uint8_t fd, char *buff, uint64_t maxBytes) {
	_int80(SYSWRITE, fd, (uint64_t)buff, maxBytes);
}

void putchar(char c) {
	_int80(SYSWRITE, STDOUT, (uint64_t)&c, 1);
}

char getchar() {
	char result;
	_int80(SYSREAD, STDIN, (uint64_t)&result, 1);
	return result;
}

uint8_t getscancode() {
	char result;
	_int80(SYSREAD, STDIN_RAW, (uint64_t)&result, 1);
	return result;
}

void clearScreen() {
	_int80(SYSCLEAR, 0, 0, 0);
}

void scroll() {
	_int80(SCROLL, 1, 0, 0);
}

void reboot() {
	_int80(REBOOT, 0, 0, 0);
}

void print(const char *str) {
	_int80(SYSWRITE, STDOUT, (uint64_t)str, strlen(str));
}

void printNum(uint64_t num) {
	char buff[20];
	intToStr(num, buff);
	print(buff);
}

uint64_t createProcess(uint64_t parentPid, char name[MAX_NAME_LENGTH], void *entryPoint, uint64_t argc, char *argv[]) {

	int i = 0;
	struct createProcessParams_s params;
	
	params.parentPid = parentPid;
	params.name = name;
	params.entryPoint = entryPoint;
	params.argc = argc;
	params.argv = argv;
	uint64_t result;
	_int80(CREATE_PROCESS, (uint64_t) &params, (uint64_t) &result, 0);
	return result;
}

uint64_t time() {
	uint64_t result;
	_int80(TIME, (uint64_t)&result, 0, 0);
	return result;
}

void paintPx(uint64_t x, uint64_t y) {
	paintColorPx(x, y, WHITE);
}

void paintColorPx(uint64_t x, uint64_t y, uint32_t color) {
	_int80(PAINT_PX_COLOR, x, y, color);
}

void paintRect(REKTangle *rekt) {
	_int80(PAINT_REKT, (uint64_t) rekt, 0, 0);
}

void fillRect(REKTangle *rekt) {
	_int80(FILL_REKT, (uint64_t) rekt, 0, 0);
}

void paintImg(Image *img) {
	_int80(PAINT_IMG, (uint64_t) img, 0, 0);
}

void sleep(uint64_t sleepTime){
	uint64_t startTime = time();
	while(time()<startTime+sleepTime);
	return;
}

void soundFX(uint32_t freq){
	_int80(SPEAKER, freq, 1, 0);
	sleep(50);
	_int80(SPEAKER, 0, 1, 0);
}

/*
void printf(const char *format, vargs *args) {
	int c;
	char buf[20];

	char *p = (char *)format;
	int argc = 0;
	while(*p) {
		if(*p == '%') {
			argc++;
		}
		p++;
	}
	if(argc != args->count) {
		return;
	}

	int argIndex = 0;
	while((c = *format++) != 0) {
		if (c != '%') {
			putchar(c);
		}
		else {
			c = *format++;
			int isSigned = 1;
			int isLong = 0;
			if(c == 'u') {
				isSigned = 0;
				c = *format++;
			}
			if(c == 'l') {
				isLong = 1;
				c = *format++;
			}
			switch (c) {
				case 'd':
				case 'i':
					if(isSigned) {
						if(isLong) {
							intToStrBase(*((int64_t *) args->args[argIndex++]), buf, 10);
						}
						else {
							intToStrBase(*((int32_t *) args->args[argIndex++]), buf, 10);	
						}
					}
					else {
						if(isLong) {
							intToStrBase(*((uint64_t *) args->args[argIndex++]), buf, 10);
						}
						else {
							intToStrBase(*((uint32_t *) args->args[argIndex++]), buf, 10);	
						}
					}
					p = buf;
					goto STRING;
					break;

				case 'x':
					if(isSigned) {
						if(isLong) {
							intToStrBase(*((int64_t *) args->args[argIndex++]), buf, 16);
						}
						else {
							intToStrBase(*((int32_t *) args->args[argIndex++]), buf, 16);	
						}
					}
					else {
						if(isLong) {
							intToStrBase(*((uint64_t *) args->args[argIndex++]), buf, 16);
						}
						else {
							intToStrBase(*((uint32_t *) args->args[argIndex++]), buf, 16);	
						}
					}
					p = buf;
					goto STRING;
					break;

				if(isSigned) {
						if(isLong) {
							intToStrBase(*((int64_t *) args->args[argIndex++]), buf, 2);
						}
						else {
							intToStrBase(*((int32_t *) args->args[argIndex++]), buf, 2);	
						}
					}
					else {
						if(isLong) {
							intToStrBase(*((uint64_t *) args->args[argIndex++]), buf, 2);
						}
						else {
							intToStrBase(*((uint32_t *) args->args[argIndex++]), buf, 2);	
						}
					}
					p = buf;
					goto STRING;
					break;

				case 'c':
					putchar(*((char *) args->args[argIndex++]));
					break;

				case 's':
					p = (char *)args->args[argIndex++];
					if(!p)
						p = "(null)";

					STRING:
					while (*p)
						putchar(*p++);
					break;

				default:
					putchar('?');
					argIndex++;
					break;
			}
		}
	}
}
*/