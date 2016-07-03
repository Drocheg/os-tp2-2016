#ifndef VIDEO_COMMON_H
#define VIDEO_COMMON_H

#include <stdint.h>

#define CHAR_PX_SIZE 8
#define WHITE 0xFFFFFF
#define BLACK 0x000000

typedef struct {
	uint64_t x;		//Top-left corner
	uint64_t y;		//Top-left corner
	uint64_t width;
	uint64_t height;
	uint32_t color;	//Hex-encoded
} REKTangle;

#pragma pack(push)
#pragma pack (1) 				//Align structure to 1 byte
typedef struct {
	uint64_t width;
	uint64_t height;
	uint8_t *pixelData;			//NOTE: When accesing this, use uint8_t* myPxPointer = &myImgStruct->pixelData (the & is intentional)
} Image;
#pragma pack(pop)

/**
* Converts the specified RGB value to a 24-bit hex value.
*/
uint32_t RGBtoHex(uint8_t r, uint8_t g, uint8_t b);

/**
* Converts the specified 24-bit hex value to an RGB value,
* storing it in RGBout as follows:
*	RGBout[0] = R
*	RGBout[1] = G
*	RGBout[2] = B
*/
void hexToRGB(uint32_t hex, uint8_t RGBout[3]);

/**
* Converts the specified 24-bit hex value to an BGR value,
* storing it in BGRout as follows:
*	BGRout[0] = B
*	BGRout[1] = G
*	BGRout[2] = R
*/
void hexToBGR(uint32_t hex, uint8_t BGRout[3]);


#endif