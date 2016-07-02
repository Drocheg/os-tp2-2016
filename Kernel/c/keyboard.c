#include <keyboard.h>
#include <libasm.h>
#include <stdlib.h>
#include <rawKeys.h>
#include <tty.h>


/* Typedefs */
typedef int64_t (*ActionToMake) (uint8_t);

/* Keyboard modes */
static KeyboardMode keyboardMode = TTY; /* default, TTY */
static uint8_t caps = 0, num = 1, shift = 0;

/* Actions */
static ActionToMake actions[2] = {ttySTDINAddElement, rawAddElement};



/* Static function prototypes */
static uint8_t processModifierKey(uint8_t scanCode);
static void actionToMake(uint8_t scanCode);


void attendKeyboard() {

	uint8_t scanCode = inb(0x60);
	processModifierKey(scanCode);
	actionToMake(scanCode);
}

void changeMode(KeyboardMode mode) {
	keyboardMode = mode;
}

uint8_t isCapsLockOn() {
	return caps;
}

uint8_t isNumLockOn() {
	return num;
}

uint8_t isShifted() {
	return shift;
}

static void actionToMake(uint8_t scanCode) {

	(actions[keyboardMode])(scanCode);
}

//Modifies appropriate flags. Returns 1 if provided scan code is a modifier key, 0 otherwise.
static uint8_t processModifierKey(uint8_t scanCode) {
	switch(scanCode) {
		case 0x2A:		//L shift (make code)
		case 0x36:		//R shift
			shift += scanCode;
			break;
		
		case 0xAA:		//L shift (break code)
		case 0xB6:		//R shift
			shift -= (scanCode - 128);
			break;

		case 0x3A:		//caps lock (ignore break code, toggle on make)
			caps = !caps;
			break;

		case 0x45:
			num = !num;
			break;
		default:
			return 0;
			break;
	}
	return 1;
}









