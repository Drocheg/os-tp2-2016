#include <keyboard.h>
#include <libasm.h>
#include <stdlib.h>
#include <scanCodes.h>
#include <video.h>

static uint8_t queue[KEYBOARD_BUFF_SIZE] = {0};
static uint8_t readIndex = -1, writeIndex = 0;
static uint8_t caps = 0, ctrl = 0, alt = 0;

uint8_t processModifierKey(uint8_t scanCode);
char processScanCode(uint8_t scanCode);

uint8_t bufferIsEmpty() {
	return readIndex == writeIndex-1 || (readIndex == KEYBOARD_BUFF_SIZE-1 && writeIndex == 0);
}

uint8_t bufferIsFull() {
	return readIndex == writeIndex;
}

//Returns the next stored scan code, if present
uint8_t pollRawKey() {
	if(bufferIsEmpty()) return 0;
	if(++readIndex >= KEYBOARD_BUFF_SIZE) {
		readIndex = 0;
	}
	return queue[readIndex];
}

//Interprets the next stored scan code, if present, and returns it
uint8_t pollProcessedKey() {
	uint8_t raw = pollRawKey();
	if(raw == 0) return 0;
	return processScanCode(raw);
}

//Adds a scan code to the end of the queue, if there is room
uint8_t offerKey(uint8_t scanCode) {

	if(bufferIsFull()) {
		return 0;
	}
	processModifierKey(scanCode);	
	queue[writeIndex++] = scanCode;
	if(writeIndex == KEYBOARD_BUFF_SIZE) {
		writeIndex = 0;
	}
	return 1;
}

//Decodes the scan code and converts letters to upper case if
//a caps key is pressed
char processScanCode(uint8_t scanCode) {
	if(scanCode >= 256) {
		return 0;
	}
	char ascii = decodeScanCode(scanCode);
	if(isAlpha(ascii)) {
		return (caps ? toUpperChar(ascii) : ascii);
	}
	return ascii;
}

//Modifies appropriate flags. Returns 1 if provided scan code is a modifier key, 0 otherwise.
uint8_t processModifierKey(uint8_t scanCode) {
	switch(scanCode) {
		case 0x2A:		//L shift (make code)
		case 0x36:		//R shift
			caps = 1;
			break;

		case 0x3A:		//caps lock (ignore break code, toggle on make)
			caps = 1-caps;
			break;
		
		case 0xAA:		//L shift (break code)
		case 0xB6:		//R shift
			caps = 0;
			break;
		
		case 0x38:		//L alt
			alt = 1;
			break;
		
		case 0xB8:		//R alt
			alt = 0;
			break;
		
		case 0x1D:		//L control
			ctrl = 1;
			break;
		
		case 0x9D:		//R control
			ctrl = 0;
			break;
		
		default:
			return 0;
			break;
	}
	return 1;
}



uint64_t stdinReadChar(uint32_t index, char *character) {
	*character = (char) pollProcessedKey();
	return 0;
}


uint64_t stdinWriteChar(uint32_t index, char *character) {
	return -1; /* Unsupported operation */
}

uint64_t stdinDataAvailable(uint32_t index) {
	
	if (bufferIsEmpty()) {
		return -1;  /* Returns -1 when there is no data available */
	}
	return 0; /* Returns 0 when there is data available */
}


uint64_t stdinHasFreeSpace(uint32_t index) {
	if (bufferIsFull()) {
		return -1;	/* Returns -1 when file doesn't have space */
	}
	return 0; /* Returns 0 when it has free space */
}








