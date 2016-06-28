/*
SONG FORMAT:

(32 bits) number of notes = n
	n times:
		(32 bits) frequency
		(8 bits) time

Example:
n=2
00000000000000000000000000000010 
f1
00000000000000000000000100000101
t1
00001010 
f2              
00000000000000000000001000001011
t2   
00001010  
*/

#include <stdint.h>
#include <stdio.h>

#pragma pack(push)
#pragma pack (1) 		/* Align struct to 1 byte */
typedef struct
{
	uint32_t freq;
	uint8_t duration;
} note;
#pragma pack(pop)

int main(int argc, char *argv[]) {
	FILE *f;
	char *fname = "song1.bin";
	f = fopen(fname,"wb");  //(w)rite (b)inary
	
	note NOTES[] = {
		{261, 1},
		{0, 1},
		{261, 1},
		{0, 1},
		{261, 1},
		{349, 8},
		{0, 2},
		{440, 5},
		{0, 1},
		{261, 1},
		{0, 1},
		{261, 1},
		{0, 1},
		{261, 1},
		{349, 8},
		{0, 2},
		{440, 5},
		{0, 15},

		{349, 3},
		{0, 1},
		{349, 2},
		{329, 1},
		{0, 1},
		{329, 1},
		{0, 1},
		{293, 1},
		{0, 1},
		{293, 1},
		{0, 1},
		{261, 3},
		{0, 10},

		{261, 1},
		{0, 1},
		{261, 1},
		{0, 1},
		{261, 1},
		{329, 5},
		{0, 5},
		{392, 5},
		{0, 1},
		{261, 1},
		{0, 1},
		{261, 1},
		{0, 1},
		{261, 1},
		{329, 5},
		{0, 5},
		{392, 10},
		{0, 10},

		{523, 5},
		{0, 1},
		{587, 1},
		{0, 1},
		{523, 1},
		{0, 1},
		{466, 1},
		{0, 1},
		{440, 1},
		{0, 1},
		{392, 1},
		{0, 1},
		{349, 5}
	};

	uint32_t n = sizeof(NOTES) / sizeof(note);

	printf("Writing %d notes:\n", n);
	int i;
	for(i = 0; i < n; i++) {
		printf("freq = %d (0x%x) ", NOTES[i].freq, NOTES[i].freq);
		printf("duration = %d (0x%x)\n", NOTES[i].duration, NOTES[i].duration);
	}

	//Write
	uint32_t songNum = 1;
	fwrite(&songNum, 4, 1, f);
	fwrite(&n, 4, 1, f);
	for(i = 0; i < n; i++) {
		fwrite(&(NOTES[i].freq), 4, 1, f);
		fwrite(&(NOTES[i].duration), 1, 1, f);
	}
	fclose(f);
	printf("Song write complete. File is %s", fname);

	printf("\n");
	return 0;
}