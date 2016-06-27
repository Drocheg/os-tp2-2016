#include <video.h>
#include <stdlib.h>

//Pure64 sets VESA mode to 1024x768, 24bit

static char buffer[64] = { '0' };
static const uint32_t width = 1024;
static const uint32_t height = 768;
static uint8_t * const video = (uint8_t*)0xFD000000;
static uint8_t * const endVideo = (uint8_t*)42/*(video  + width*height)*/;
static uint8_t * currentVideo = (uint8_t*)0xFD000000;


void paintPixel(uint64_t x, uint64_t y, uint8_t red, uint8_t green, uint8_t blue) {
	char *videoPointer = (char*) video;
    uint64_t offset = ((y*width + x))*3;
    videoPointer = videoPointer + offset;
    *videoPointer = blue;
    videoPointer++;
    *videoPointer = green;
    videoPointer++;
    *videoPointer = red;
}

//Prints the specified message to screen.
void ncPrint(const char * string)
{
	int i;
	for(i = 0; string[i] != 0; i++) {
		ncPrintChar(string[i]);
	}
}

void ncPrintln(const char * string) {
	ncPrint(string);
	ncNewline();
}

//Prints the specified message to screen with the specified color
void ncPrintColor(const char * string, char color) {
	int i;
	for (i = 0; string[i] != 0; i++) {
		ncPrintColorChar(color, string[i]);
	}
}

void ncPrintlnColor(const char * string, char color) {
	ncPrintColor(string, color);
	ncNewline();
}

//Prints the scpecified char to screen. Handles special chars and
//scrolls if necessary
void ncPrintChar(char character)
{
	if(character == '\n') {
		ncNewline();
	}
	else if(character == '\b')	{
		ncBackspace();
	}
	else {
		*currentVideo = character;
		currentVideo += 2;
	}
	if(currentVideo >= endVideo) {
		ncScroll();
		currentVideo = video+(width*2*(height-1));
	}
}

void ncPrintColorChar(char character, char color)
{
	if(character == '\n') {
		do 
		{
			*(currentVideo++) = ' ';
			*(currentVideo++) = color;
		}
		while((uint64_t)(currentVideo - video) % (width * 2) != 0);
	}
	else {
		*(currentVideo++) = character;
		*(currentVideo++) = color;
	}
	if(currentVideo >= endVideo) {
		ncScroll();
		currentVideo = video+(width*2*(height-1));
	}
}

void ncNewline()
{
	do 
	{
		ncPrintChar(' ');
	}
	while((uint64_t)(currentVideo - video) % (width * 2) != 0);
}

void ncBackspace() {
	if(currentVideo > video) {
		currentVideo -= 2;		//Goes back if possible
	}
	*currentVideo = 0;			//Clears char
}

void ncPrintDec(uint64_t value)
{
	ncPrintBase(value, 10);
}

void ncPrintHex(uint64_t value)
{
	ncPrintBase(value, 16);
}

void ncPrintBin(uint64_t value)
{
	ncPrintBase(value, 2);
}

void ncPrintBase(uint64_t value, uint32_t base)
{
    intToStrBase(value, buffer, base);
    ncPrint(buffer);
}

void ncClear()
{
	int i = 0;

	while(i < height*width*2) {
		video[i++] = ' ';
		video[i++] = 0x07;
	}
	currentVideo = video;
}

void ncScroll() {
	ncScrollLines(1);
}

void ncScrollLines(uint8_t lines) {
	if(lines > height) {
		ncClear();
		return;
	}
	uint32_t current = (width*2*lines),
				end = (width*2*height)-2-current,
				i;
	//Move content up
	for(i = 0; video+current < endVideo; current += 2, i += 2) {
		video[i] = video[current];
	}
	//Clear bottom
	for(i = (width*2*height)-2; i > end; i -= 2) {
		video[i] = 0;
	}
}

//Modifies the color bytes of the video screen
void ncRAINBOWWWWWW() {
	uint8_t *v = video;
	int i;
	for(i = 1; i < (80*25*2)-1; i += 2) {
		v[i] = (char)i;
	}
	v = video + (80*2*12) + (27*2);
	//Print in the middle of the screen
	char *message = "TASTE THE RAINBOWWWWWWWWWW";
	for(i = 0; message[i] != 0; i++) {
		*v = message[i];
		v += 2;
	}
}