#include <video2.h>
#include <libasm.h>

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
#define VIDEO_ADDRESS_START ((char *)0xFD000000)
#define RESOLUTION 2359296
#define VIDEO_RESOLUTION 786432
#define END_OF_SCREEN ((char*)0xFD000000) + 2359296
#define WIDTH 1024
#define HEIGHT 768
#define BPP_32_FACTOR 4
#define DATA_MODULE_ADDRESS 0x500000

/*

    Credit where credit is due: 90% of this code is from Flacu at
    https://bitbucket.org/Flacugf/tpe-arqui/src/cc7f0554eaaf5809d88c69348596cc147e131fc2/TPE/Kernel/video.c?at=master&fileviewer=file-view-default

*/


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
    int offset = ((y*WIDTH + x))*BPP_32_FACTOR;
    videoPointer = videoPointer + offset;
    *videoPointer = blue;
    videoPointer++;
    *videoPointer = green;
    videoPointer++;
    *videoPointer = red;
}