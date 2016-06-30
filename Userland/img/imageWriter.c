#include <stdint.h>
#include <stdio.h>
//#include "firanofaurio.c"
#include "dinosaurio.c"
int main(int argc, char *argv[]) {

	/**
	FILE *f;
	char *fname = "firanofaurio.img";
	f = fopen(fname,"wb");  //(w)rite (b)inary
	
	printf("Writing image data for %dx%d image:\n", firanofaurio.width, firanofaurio.height);
	

	//Write
	fwrite(&(firanofaurio.width), sizeof(firanofaurio.width), 1, f);	//Width
	fwrite(&(firanofaurio.height), sizeof(firanofaurio.height), 1, f);	//Height
	uint8_t *data = firanofaurio.pixel_data;
	for(int y = 0; y < firanofaurio.height; y++) {
		for(int x = 0; x < firanofaurio.width; x++) {
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
	
	printf("Writing image data for %dx%d image:\n", dinosaurio.width, dinosaurio.height);
	

	//Write
	fwrite(&(dinosaurio.width), sizeof(dinosaurio.width), 1, f);	//Width
	fwrite(&(dinosaurio.height), sizeof(dinosaurio.height), 1, f);	//Height
	uint8_t *data = dinosaurio.pixel_data;
	for(int y = 0; y < dinosaurio.height; y++) {
		for(int x = 0; x < dinosaurio.width; x++) {
			fwrite(data++, sizeof(uint8_t), 1, f);	//Red
			fwrite(data++, sizeof(uint8_t), 1, f);	//Green
			fwrite(data++, sizeof(uint8_t), 1, f);	//Blue
		}
	}
	fclose(f);
	printf("Image write complete. File is %s\n", fname);
	return 0;
}