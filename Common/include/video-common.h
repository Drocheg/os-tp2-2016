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

typedef struct {
	uint64_t x;		//Top-left corner
	uint64_t y;		//Top-left corner
	uint64_t width;
	uint64_t height;
	uint32_t color;
	void *data;
} Image;

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