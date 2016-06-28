#include <video.h>
#include <stdlib.h>

extern const uint32_t width;
extern const uint32_t height;
void paintCharRGB(uint64_t x, uint64_t y, uint8_t c, uint8_t r, uint8_t g, uint8_t b);
void paintChar(uint64_t x, uint64_t y, uint8_t c);


static uint64_t x = 8, y = 0;
static char buffer[64] = { '0' };


void ncPrintColorChar(char character, uint8_t r, uint8_t g, uint8_t b) {
	if(character == '\n') {
		ncNewline();
	}
	else if(character == '\b')	{
		ncBackspace();
	}
	else {
		paintCharRGB(x, y, character, r, g, b);
		x += 8;
	}
	if(x >= width) {
		if(y >= height) {
			ncScroll();
			x = 8;
			y = height-8;
		}
		else {
			x = 8;
			y += 8;
		}
	}
}

void ncPrintChar(char character) {
	ncPrintColorChar(character, 255, 255, 255);
}

void ncPrintColor(const char * string, uint8_t r, uint8_t g, uint8_t b) {
	for (int i = 0; string[i] != 0; i++) {
		ncPrintColorChar(string[i], r, g, b);
	}
}

void ncPrint(const char * string) {
	ncPrintColor(string, 255, 255, 255);
}

void ncPrintlnColor(const char * string, uint8_t r, uint8_t g, uint8_t b) {
	ncPrintColor(string, r, g, b);
	ncNewline();
}

void ncPrintln(const char * string) {
	ncPrintlnColor(string, 255, 255, 255);
}

void ncNewline() {
	uint64_t startY = y;
	do {
		ncPrintColorChar(' ', 0, 0, 0);
	}
	while(y == startY);
}

//Scrolls back 1 character, wrapping up to the previous line if necessary.
void goBack() {
	if(x > 8) {
		x -= 8;
	}
	else if(y > 0) {
		x = width - 8;
		y -= 8;
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
	x = 8;
	y = 0;
}

void ncScroll() {
	ncScrollLines(1);
}

void ncScrollLines(uint8_t lines) {
	if(lines > height) {
		ncClear();
		return;
	}
	// TODO
	// uint32_t current = (width*2*lines),
	// 			end = (width*2*height)-2-current,
	// 			i;
	// //Move content up
	// for(i = 0; video+current < endVideo; current += 2, i += 2) {
	// 	video[i] = video[current];
	// }
	// //Clear bottom
	// for(i = (width*2*height)-2; i > end; i -= 2) {
	// 	video[i] = 0;
	// }
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
