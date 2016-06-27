#include <video.h>
#include <stdlib.h>
#include <libasm.h>
#include <stdlib.h>
#include <bitmaps.h>
#include <kernel-lib.h>

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
//Bytes per pixel
#define BPP 4
//Bytes per line
#define BPL ((WIDTH)*(BPP))


void BgaWriteRegister(unsigned short, unsigned short);
unsigned short BgaReadRegister(unsigned short);
int BgaIsAvailable(void);
void BgaSetVideoMode(unsigned int, unsigned int ,unsigned int ,int ,int);
void BgaSetBank(unsigned short);


#define WHITE 255, 255, 255
#define BLACK 0, 0, 0
const uint32_t width = WIDTH;
const uint32_t height = HEIGHT;

void paintPixelRGB(uint64_t x, uint64_t y, uint8_t red, uint8_t green, uint8_t blue) {
    if(x < 0 || x >= width || y < 0 || y >= height) {
    	return;
    }
    uint8_t *pixelPtr = (uint8_t*) VIDEO_ADDRESS_START;
    pixelPtr += (y*width + x)*BPP;
    pixelPtr[0] = blue;
    pixelPtr[1] = green;
    pixelPtr[2] = red;
}

void paintPixel(uint64_t x, uint64_t y) {
	paintPixelRGB(x, y, WHITE);
}

//Paints the specified character at the specified position (if valid), in the specified RGB color.
void paintCharRGB(uint64_t x, uint64_t y, char c, uint8_t r, uint8_t g, uint8_t b) {
    static const int mask[8]={1,2,4,8,16,32,64,128};
    uint8_t *bitmap = bitmaps[(uint8_t)c];

    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 8; col++) {
            int visible = bitmap[row] & mask[col];
            if(visible) {
                paintPixelRGB(x-col, y+row, r, g, b);
            }
            else {
                paintPixelRGB(x-col, y+row, BLACK);
            }
        }
    }
}

//Paints the specified character at the specified position (if valid) in white.
void paintChar(uint64_t x, uint64_t y, char c) {
    paintCharRGB(x, y, c, WHITE);
}


//Paints the specified string at the specified position (if valid) in the specified RGB color.
void paintStrRGB(uint64_t x, uint64_t y, const char* str, uint8_t r, uint8_t g, uint8_t b) {
    for(int i = 0; i < strlen(str); i++) {
        paintChar(x+(i*8), y, str[i]);
    }
}

//Paints the specified string at the specified position (if valid) in white.
void paintStr(uint64_t x, uint64_t y, const char* str) {
    paintStrRGB(x, y, str, WHITE);
}

void clear() {
    memset(VIDEO_ADDRESS_START, 0, WIDTH*HEIGHT*BPP);
}

void setGraphicMode() {
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
