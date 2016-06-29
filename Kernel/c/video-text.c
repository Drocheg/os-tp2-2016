#include <video-common.h>
#include <video.h>
#include <stdlib.h>

extern const uint32_t width;
extern const uint32_t height;
void paintColorChar(uint64_t x, uint64_t y, uint8_t c, uint32_t color);
void paintChar(uint64_t x, uint64_t y, uint8_t c);


static uint64_t x = CHAR_PX_SIZE, y = 0;
static char buffer[64] = { '0' };


void ncPrintColorChar(char character, uint32_t color) {
	if(character == '\n') {
		ncNewline();
	}
	else if(character == '\b')	{
		ncBackspace();
	}
	else {
		paintColorChar(x, y, character, color);
		x += CHAR_PX_SIZE;
	}
	if(x >= width) {
		if(y >= height-CHAR_PX_SIZE) {
			ncScroll();
			x = CHAR_PX_SIZE;
			y = height-CHAR_PX_SIZE*2;
			// Content scrolled, now clear last line (can't call ncNewLine or we will have infinite recursion, as we will fall back here on the last character)
			REKTangle r = {CHAR_PX_SIZE, height-CHAR_PX_SIZE, width, CHAR_PX_SIZE, BLACK};
			fillRect(&r);
		}
		else {
			x = CHAR_PX_SIZE;
			y += CHAR_PX_SIZE;
		}
	}
}

void ncPrintChar(char character) {
	ncPrintColorChar(character, WHITE);
}

void ncPrintColor(const char * string, uint32_t color) {
	for (int i = 0; string[i] != 0; i++) {
		ncPrintColorChar(string[i], color);
	}
}

void ncPrint(const char * string) {
	ncPrintColor(string, WHITE);
}

void ncPrintlnColor(const char * string, uint32_t color) {
	ncPrintColor(string, color);
	ncNewline();
}

void ncPrintln(const char * string) {
	ncPrintlnColor(string, WHITE);
}

void ncNewline() {
	uint64_t startY = y;
	do {
		ncPrintColorChar(' ', BLACK);
	}
	while(y == startY);
}

//Scrolls back 1 character, wrapping up to the previous line if necessary.
void goBack() {
	if(x > CHAR_PX_SIZE) {
		x -= CHAR_PX_SIZE;
	}
	else if(y > 0) {
		x = width - CHAR_PX_SIZE;
		y -= CHAR_PX_SIZE;
	}
}

void ncBackspace() {
	goBack();
	ncPrintChar(0);
	goBack();
}

void ncPrintDec(uint64_t value) {
	ncPrintBase(value, 10);
}

void ncPrintHex(uint64_t value) {
	ncPrintBase(value, 16);
}

void ncPrintBin(uint64_t value) {
	ncPrintBase(value, 2);
}

void ncPrintBase(uint64_t value, uint32_t base) {
    intToStrBase(value, buffer, base);
    ncPrint(buffer);
}

void ncClear() {
	clear();
	x = CHAR_PX_SIZE;
	y = 0;
}

void ncScroll() {
	ncScrollLines(1);
}

void ncScrollLines(uint8_t lines) {
	if(lines > height) {
		lines = height;
	}
	shiftUp(lines*CHAR_PX_SIZE);
}

//Modifies the color bytes of the video screen
void ncRAINBOWWWWWW() {
	// uint8_t *v = video;
	// int i;
	// for(i = 1; i < (80*25*2)-1; i += 2) {
	// 	v[i] = (char)i;
	// }
	// v = video + (80*2*12) + (27*2);
	// //Print in the middle of the screen
	// char *message = "TASTE THE RAINBOWWWWWWWWWW";
	// for(i = 0; message[i] != 0; i++) {
	// 	*v = message[i];
	// 	v += 2;
	// }
}
