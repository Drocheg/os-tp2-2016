#include <video-common.h>

uint32_t RGBtoHex(uint8_t r, uint8_t g, uint8_t b) {
	uint32_t result = 0;
	result |= /*(uint32_t)*/ r;
	result <<= 8;
	result |= /*(uint32_t)*/ g;
	result <<= 8;
	result |= /*(uint32_t)*/ b;
	return result;
}

void hexToRGB(uint32_t hex, uint8_t RGBout[3]) {
	RGBout[0] = (uint8_t) (hex >> 16);
	RGBout[1] = (uint8_t) (hex >> 8);
	RGBout[2] = (uint8_t) hex;
}

void hexToBGR(uint32_t hex, uint8_t BGRout[3]) {
	BGRout[2] = (uint8_t) (hex >> 16);
	BGRout[1] = (uint8_t) (hex >> 8);
	BGRout[0] = (uint8_t) hex;
}