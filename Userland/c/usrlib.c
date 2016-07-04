#include <usrlib.h>
#include <stdlib.h>
#include <interrupts.h>
#include <syscalls.h>
#include <fileDescriptors.h>
#include <scanCodes.h>
#include <video-common.h>
#include <stdint.h>
#include <file-common.h>
#include <stdint.h>
#include <fileDescriptors.h>
#include <mq.h>
#include <libasm.h>
#include <unistd.h>


// void fread(uint8_t fd, char *buff, uint64_t maxBytes) {
// 	_int80(SYSREAD, fd, (uint64_t)buff, maxBytes);
// }

// void fwrite(uint8_t fd, char *buff, uint64_t maxBytes) {
// 	_int80(SYSWRITE, fd, (uint64_t)buff, maxBytes);
// }

// void putchar(char c) {
// 	_int80(SYSWRITE, STDOUT_, (uint64_t)&c, 1);
// }

// char getchar() {
// 	char result;
// 	_int80(SYSREAD, STDIN_, (uint64_t)&result, 1);
// 	return result;
// }

void ps_sys(){
	_int80(PS, 0, 0, 0);
}

void ipcs_sys(){
	_int80(IPCS, 0, 0, 0);
}

uint8_t getScanCode() {
	uint8_t result;
	read(RAW_KEYS, (char *) &result, 1);
	return result;
}

void clearScreen() {
	_int80(SYSCLEAR, 0, 0, 0);
}

void scroll() {
	_int80(SCROLL, 1, 0, 0);
}

void print(const char *str) {
	_int80(SYSWRITE, STDOUT_, (uint64_t)str, strlen(str));
}

void printNum(int64_t num) {
	char buff[21];
	int negative = num < 0;
	if(negative) {
		buff[0] = '-';
	}
	intToStr(num, negative ? buff+1 : buff);
	print(buff);
}

int64_t createProcess(char name[MAX_NAME_LENGTH], void *entryPoint, uint64_t argc, char *argv[]) {


	struct createProcessParams_s params;	
	params.name = name;
	params.entryPoint = entryPoint;
	params.argc = argc;
	params.argv = argv;
	int64_t result;
	_int80(CREATE_PROCESS, (uint64_t) &params, (uint64_t) &result, 0);
	return result;
}

// uint64_t time() {
// 	uint64_t result;
// 	_int80(TIME, (uint64_t)&result, 0, 0);
// 	return result;
// }

void * malloc(uint64_t size){
	void * result;
	_int80(MALLOC, (uint64_t) &result, size, 0);
	return result;
}

void paintPx(uint64_t x, uint64_t y) {
	paintColorPx(x, y, WHITE);
}

int64_t waitpid(uint64_t pid) {
	int64_t result = 0;
	_int80(WAITPID, pid, (uint64_t) &result, 0);
	return result;
}

void yield() {
	_int80(YIELD, 0, 0, 0);
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

void paintImg(Image *img, uint64_t x, uint64_t y) {
	_int80(PAINT_IMG, (uint64_t) img, x, y);
}

// void sleep(uint64_t miliseconds) {
// 	_int80(SLEEP, miliseconds, 0, 0);
// 	//sleepCroto(miliseconds);
// }

void sleepCroto(uint64_t miliseconds){
	uint64_t loopTime=time();
	while(time()<loopTime+miliseconds);
}

void soundFX(uint32_t freq){
	_int80(SPEAKER, freq, 1, 0);
	sleep_sys(50);
	_int80(SPEAKER, 0, 1, 0);
}

void exit(int64_t result){
	_int80(EXIT, result,0,0);
}

void changeToScanCodes() {
	_int80(CHANGE_KBD_MODE, RAW, 0, 0);
}

void changeToKeys() {
	_int80(CHANGE_KBD_MODE, TTY, 0, 0);
}

char* concat(char *s1, char *s2)
{
    int64_t len1 = strlen(s1);
    int64_t len2 = strlen(s2);
    char *result = malloc(len1+len2+1);//+1 for the zero-terminator
    if(result==NULL) return NULL;
    memcpy(result, s1, len1);
    memcpy(result+len1, s2, len2+1);//+1 to copy the null-terminator
    return result;
}

void stdoutFFlush() {
	_int80(STDOUT_FFLUSH, 0, 0, 0);
}

void stderrFFlush() {
	_int80(STDERR_FFLUSH, 0, 0, 0);
}
