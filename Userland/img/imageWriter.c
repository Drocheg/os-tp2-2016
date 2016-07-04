#include <stdint.h>
#include <stdio.h>

#include "dinosaurio.c"


int main(int argc, char *argv[]) {

	/**
	FILE *f;
	char *fname = "img1.img";
	f = fopen(fname,"wb");  //(w)rite (b)inary
	
	uint64_t width = (uint64_t) gimp_image.width,
			height = (uint64_t) gimp_image.height;

	printf("Writing image data for %dx%d image:\n", width, height);
	

	//Write
	fwrite(&width, sizeof(width), 1, f);	//Width
	fwrite(&height, sizeof(height), 1, f);	//Height
	uint8_t *data = gimp_image.pixel_data;
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			fwrite(data++, sizeof(uint8_t), 1, f);	//Red
			fwrite(data++, sizeof(uint8_t), 1, f);	//Green
			fwrite(data++, sizeof(uint8_t), 1, f);	//Blue
		}
	}
	fclose(f);
	printf("Image write complete. File is %s\n", fname);
	return 0;
	**/
	FILE *f;
	char *fname = "dinosaurio.img";
	f = fopen(fname,"wb");  //(w)rite (b)inary
	
	uint64_t width = (uint64_t) gimp_image.width,
			height = (uint64_t) gimp_image.height;

	printf("Writing image data for %dx%d image:\n", width, height);
	

	//Write
	fwrite(&width, sizeof(width), 1, f);	//Width
	fwrite(&height, sizeof(height), 1, f);	//Height
	uint8_t *data = gimp_image.pixel_data;
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			fwrite(data++, sizeof(uint8_t), 1, f);	//Red
			fwrite(data++, sizeof(uint8_t), 1, f);	//Green
			fwrite(data++, sizeof(uint8_t), 1, f);	//Blue
		}
	}
	fclose(f);
	printf("Image write complete. File is %s\n", fname);
	return 0;
}