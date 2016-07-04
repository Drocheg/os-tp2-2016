#include <video.h>
#include <stdlib.h>
#include <libasm.h>
#include <stdlib.h>
#include <bitmaps.h>
#include <kernel-lib.h>
#include <video-common.h>


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


const uint32_t width = WIDTH;
const uint32_t height = HEIGHT;

uint8_t * findPixel(uint64_t x, uint64_t y) {
    if(x < 0 || x >= width || y < 0 || y >= height) {
        return 0;
    }
    return (uint8_t*) VIDEO_ADDRESS_START + (y*width + x)*BPP;
}

void paintColorPixel(uint64_t x, uint64_t y, uint32_t color) {
    uint8_t *pixelPtr = findPixel(x, y);
    if(pixelPtr  == 0) {
        return;
    }
    hexToBGR(color, pixelPtr);
    // pixelPtr[0] = blue;
    // pixelPtr[1] = green;
    // pixelPtr[2] = red;
}

void paintPixel(uint64_t x, uint64_t y) {
   paintColorPixel(x, y, WHITE);
}

//Paints the specified character at the specified position (if valid), in the specified color.
void paintColorChar(uint64_t x, uint64_t y, char c, uint32_t color) {
    static const int mask[8]={1,2,4,8,16,32,64,128};
    uint8_t *bitmap = bitmaps[(uint8_t)c];

    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 8; col++) {
            int visible = bitmap[row] & mask[col];
            if(visible) {
                paintColorPixel(x-col, y+row, color);
            }
            else {
                paintColorPixel(x-col, y+row, BLACK);
            }
        }
    }
}

//Paints the specified character at the specified position (if valid) in white.
void paintChar(uint64_t x, uint64_t y, char c) {
    paintColorChar(x, y, c, WHITE);
}


//Paints the specified string at the specified position (if valid) in the specified color.
void paintColorStr(uint64_t x, uint64_t y, const char* str, uint32_t color) {
    for(int i = 0; i < strlen(str); i++) {
        paintColorChar(x+(i*8), y, str[i], color);
    }
}

//Paints the specified string at the specified position (if valid) in white.
void paintStr(uint64_t x, uint64_t y, const char* str) {
    paintColorStr(x, y, str, WHITE);
}

void fillRect(REKTangle *rekt) {
    uint8_t *pixel = findPixel(rekt->x, rekt->y);
    if(pixel == 0) {
        return;
    }
    uint8_t bgr[3];
    hexToBGR(rekt->color, bgr);
    for (int row = 0; row < rekt->height; row++) {
        for (int col = 0; col < rekt->width; col++) {
            pixel[col*BPP] = bgr[0];
            pixel[col*BPP + 1] = bgr[1];
            pixel[col*BPP + 2] = bgr[2];
        }
        pixel += BPL;
    }
}

void drawRect(REKTangle *rekt) {
    REKTangle tmp = {rekt->x, rekt->y, rekt->width, 1, rekt->color};    //Top
    fillRect(&tmp);
    tmp = (REKTangle) {rekt->x, rekt->y, 1, rekt->height, rekt->color};                 //Left
    fillRect(&tmp);
    tmp = (REKTangle) {rekt->x, rekt->y+rekt->height, rekt->width, 1, rekt->color};     //Bottom
    fillRect(&tmp);
    tmp = (REKTangle) {rekt->x+rekt->width, rekt->y, 1, rekt->height, rekt->color};     //Right
    fillRect(&tmp);
}

void clear() {
    //Memset sets byte by byte, we will set word by word (8 bytes at a time)
    uint64_t *mem = (uint64_t *)VIDEO_ADDRESS_START;
    for(uint64_t offset = 0; offset < BPL*HEIGHT/sizeof(uint64_t); offset++) {
        mem[offset] = 0;
    }
}

void shiftUp(uint64_t pixels) {
    //Copy 8 bytes at a time
    uint64_t *mem = (uint64_t *) VIDEO_ADDRESS_START,
            offset = 0,
            limit = (BPL*(HEIGHT-pixels))/sizeof(uint64_t);
    //Move content up - in each iteration, put in this place what is exactly "pixels" lines below
    while(offset <= limit) {
        mem[offset] = mem[offset+WIDTH*pixels];
        offset++;
    }
}

void shiftDown(uint64_t pixels) {
    uint64_t *mem = (uint64_t *) VIDEO_ADDRESS_START,
            offset = (BPL*HEIGHT)/sizeof(uint64_t),
            limit = BPL*pixels/sizeof(uint64_t);
    while(offset >= limit) {
        mem[offset] = mem[offset-WIDTH*pixels];
        offset--;
    }
}

void paintImg(Image *img, uint64_t x, uint64_t y) {
    if(findPixel(x, y) == 0) {
        return;
    }
    uint8_t *la = (uint8_t *)&img->pixelData; //Simply using img->pixelData won't work, will dereference the first pixel color and take it as an address. Hence the &
    for(uint64_t row = 0; row < img->height; row++) {
        for(uint64_t col = 0; col < img->width; col++) {
            uint8_t *pixel = findPixel(x+col, y+row);
            if(pixel != 0) {
                pixel[2] = *(la++);
                pixel[1] = *(la++);
                pixel[0] = *(la++);
                // pixel += 4;    //Skip the 4th pixel, ignored
            }
        }
    }
}

// void shiftLeft(uint64_t pixels) {
//     uint8_t *mem = VIDEO_ADDRESS_START;
//     uint64_t offset = 0,
//             limit = (BPL*HEIGHT)-pixels;
//     while(offset <= limit) {
//         mem[offset] = mem[offset+pixels];
//         //Done with this line?
//         if(offset % (width-pixels) == 0) {
//             offset += BPL - ((width-pixels)*BPP);
//         }
//         else {
//             offset++;
//         }
//     }
// }

// void shiftRight(uint64_t pixels) {
//     uint8_t *mem = VIDEO_ADDRESS_START;
//     uint64_t offset = BPL*HEIGHT,
//             limit = pixels*BPP;
//     while(offset >= limit) {
//         mem[offset] = mem[offset-pixels];
//         //Done with this line?
//         if(offset % (width-pixels) == 0) {
//             offset -= BPL - ((width-pixels)*BPP);
//         }
//         else {
//             offset--;
//         }
//     }
//  }

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
