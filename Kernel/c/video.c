#include <video.h>
#include <stdlib.h>
#include <libasm.h>
#include <stdlib.h>
#include <bitmaps.h>

//Pure64 sets VESA mode to 1024x768, 24bit

static char buffer[64] = { '0' };
static const uint32_t width = 1024;
static const uint32_t height = 768;
static uint8_t * const video = (uint8_t*)0xFD000000;
static uint8_t * const endVideo = (uint8_t*)42/*(video  + width*height)*/;
static uint8_t * currentVideo = (uint8_t*)0xFD000000;
static uint64_t x = 8, y = 0;

void paintChar(int x, int y, char c);
void paintStr(int x, int y, const char* str);


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
		paintChar(x, y, character);
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
	uint64_t startY = y;
	do 
	{
		ncPrintChar(' ');
	}
	while(y == startY);
}

void ncBackspace() {
	if(x > 8) {
		x -= 8;
		ncPrintChar(0);
	}
	else if(y > 0) {
		x = width - 8;
		y--;
		ncPrintChar(0);
	}
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
	for(uint64_t y2 = 0; y2 < height; y2 +=8) {
		for(uint64_t x2 = 8; x2 < width; x2 +=8) {
			paintChar(x2, y2, 0);
		}
	}
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


/*
    Credit where credit is due: 90% of the code below is from Flacu, retrieved from
    https://bitbucket.org/Flacugf/tpe-arqui/src/cc7f0554eaaf5809d88c69348596cc147e131fc2/TPE/Kernel/video.c?at=master&fileviewer=file-view-default
*/
#define VBE_DISPI_INDEX_ID 0
#define VBE_DISPI_INDEX_XRES 1
#define VBE_DISPI_INDEX_YRES 2
#define VBE_DISPI_INDEX_BPP 3
#define VBE_DISPI_INDEX_ENABLE 4
#define VBE_DISPI_INDEX_BANK 5
#define VBE_DISPI_BANK_ADDRESS 0xA0000
#define VBE_DISPI_LFB_PHYSICAL_ADDRESS 0xE0000000
#define VBE_DISPI_INDEX_VIRT_WIDTH 6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 7
#define VBE_DISPI_INDEX_X_OFFSET 8
#define VBE_DISPI_INDEX_Y_OFFSET 9
#define VBE_DISPI_BPP_4 0x04
#define VBE_DISPI_BPP_8 0x08
#define VBE_DISPI_BPP_15 0x0F
#define VBE_DISPI_BPP_16 0x10
#define VBE_DISPI_BPP_24 0x18
#define VBE_DISPI_BPP_32 0x20
#define VBE_DISPI_LFB_ENABLED 0x40
#define VBE_DISPI_NOCLEARMEM 0x80
#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA 0x01CF
#define VBE_DISPI_DISABLED 0x00
#define VBE_DISPI_ENABLED 0x01
#define VBE_DISPI_ID4 0xB0C4
#define VIDEO_ADDRESS_START ((uint8_t *)0xFD000000)
#define RESOLUTION 2359296
#define VIDEO_RESOLUTION 786432
#define END_OF_SCREEN ((uint8_t*)0xFD000000) + 2359296
#define WIDTH 1024
#define HEIGHT 768
#define BPP_32_FACTOR 4
#define DATA_MODULE_ADDRESS 0x500000


void BgaWriteRegister(unsigned short, unsigned short);
unsigned short BgaReadRegister(unsigned short);
int BgaIsAvailable(void);
void BgaSetVideoMode(unsigned int, unsigned int ,unsigned int ,int ,int);
void BgaSetBank(unsigned short);

void SetVideoMode() {
   	BgaSetVideoMode(WIDTH, HEIGHT, VBE_DISPI_BPP_32, 1,1);
}

void BgaWriteRegister(unsigned short IndexValue, unsigned short DataValue) {
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
}

unsigned short BgaReadRegister(unsigned short IndexValue) {
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
    return inw(VBE_DISPI_IOPORT_DATA);
}

int BgaIsAvailable(void) {
    return (BgaReadRegister(VBE_DISPI_INDEX_ID) == VBE_DISPI_ID4);
}

void BgaSetVideoMode(unsigned int Width, unsigned int Height, unsigned int BitDepth, int UseLinearFrameBuffer, int ClearVideoMemory) {
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    BgaWriteRegister(VBE_DISPI_INDEX_XRES, Width);
    BgaWriteRegister(VBE_DISPI_INDEX_YRES, Height);
    BgaWriteRegister(VBE_DISPI_INDEX_BPP, BitDepth);
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED |
        (UseLinearFrameBuffer ? VBE_DISPI_LFB_ENABLED : 0) |
        (ClearVideoMemory ? 0 : VBE_DISPI_NOCLEARMEM));
}

void BgaSetBank(unsigned short BankNumber) {
    BgaWriteRegister(VBE_DISPI_INDEX_BANK, BankNumber);
}


void paintPixel2(int x, int y, char red, char green, char blue) {
    char *videoPointer = (char*) VIDEO_ADDRESS_START;
    int offset = (y*WIDTH + x)*BPP_32_FACTOR;
    videoPointer = videoPointer + offset;
    *videoPointer = blue;
    videoPointer++;
    *videoPointer = green;
    videoPointer++;
    *videoPointer = red;
}

//http://wiki.osdev.org/Drawing_In_Protected_Mode
//http://wiki.osdev.org/VGA_Fonts
void paintChar(int x, int y, char c) {
    uint8_t *bitmap = bitmaps[(uint8_t)c];
    int mask[8]={1,2,4,8,16,32,64,128};

    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 8; col++) {
            int visible = bitmap[row] & mask[col];
            if(visible) {
                paintPixel2(x-col, y+row, 255, 255, 255);
            }
            else {
                paintPixel2(x+col, y+row, 0, 0, 0);
            }
        }
    }
}

void paintStr(int x, int y, const char* str) {
    for(int i = 0; i < strlen(str); i++) {
        paintChar(x+(i*8), y, str[i]);
    }
}

//TODO
void paintcharPRO(int x, int y, char c, uint8_t fg, uint8_t bg) {
    // uint8_t row_data;
    // uint32_t mask1, mask2;
    // uint8_t *font_data_for_char = font1[(uint8_t)c];
    // uint32_t packed_foreground = (fg << 24) | (fg << 16) | (fg << 8) | fg;
    // uint32_t packed_background = (bg << 24) | (bg << 16) | (bg << 8) | bg;
    // uint8_t * pixel = VIDEO_ADDRESS_START + (y*WIDTH + x)*BPP_32_FACTOR;

    // for (int row = 0; row < 8; row++) {
    //     row_data = font_data_for_char[row];
    //     mask1 = font_data_lookup_table[row_data >> 16];
    //     mask2 = font_data_lookup_table[row_data & 0x0F];
    //     *(uint32_t *)pixel = (packed_foreground & mask1) | (packed_background & ~mask1);
    //     *(uint32_t *)(&pixel[4]) = (packed_foreground & mask2) | (packed_background & ~mask2);
    //     pixel += HEIGHT*BPP_32_FACTOR;
    // }

}
