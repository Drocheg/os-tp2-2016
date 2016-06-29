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
	uint32_t duration;
} note;
#pragma pack(pop)

int main(int argc, char *argv[]) {
	FILE *f;
	char *fname = "song1.bin";
	f = fopen(fname,"wb");  //(w)rite (b)inary
	
	note NOTES[] = {
		{261, 55},
		{0, 55},
		{261, 55},
		{0, 55},
		{261, 55},
		{349, 440},
		{0, 110},
		{440, 275},
		{0, 55},
		{261,55},
		{0,55},
		{261,55},
		{0,55},
		{261,55},
		{349, 440},
		{0, 110},
		{440, 275},
		{0, 825},

		{349, 165},
		{0,55},
		{349, 110},
		{329,55},
		{0,55},
		{329,55},
		{0,55},
		{293,55},
		{0,55},
		{293,55},
		{0,55},
		{261, 165},
		{0,550},

		{261,55},
		{0,55},
		{261,55},
		{0,55},
		{261,55},
		{329, 275},
		{0, 275},
		{392, 275},
		{0,55},
		{261,55},
		{0,55},
		{261,55},
		{0,55},
		{261,55},
		{329, 275},
		{0, 275},
		{392,550},
		{0,550},

		{523, 275},
		{0,55},
		{587,55},
		{0,55},
		{523,55},
		{0,55},
		{466,55},
		{0,55},
		{440,55},
		{0,55},
		{392,55},
		{0,55},
		{349, 275}
	};

	uint32_t n = sizeof(NOTES) / sizeof(note);

	printf("Writing %d notes:\n", n);
	int i;
	for(i = 0; i < n; i++) {
		printf("freq = %d (0x%x) ", NOTES[i].freq, NOTES[i].freq);
		printf("duration = %d (0x%x)\n", NOTES[i].duration, NOTES[i].duration);
	}

	//Write
	uint32_t songNum = 2; //TODO cambiar magicNum
	fwrite(&songNum, 4, 1, f);
	//MainSong
	fwrite(&n, 4, 1, f);
	for(i = 0; i < n; i++) {
		fwrite(&(NOTES[i].freq), 4, 1, f);
		fwrite(&(NOTES[i].duration), 4, 1, f);
	}

	//ExtraSong
	uint32_t m = n/2; 
	fwrite(&m, 4, 1, f);
	for(i = 0; i < m; i++) {
		fwrite(&(NOTES[i].freq), 4, 1, f);
		fwrite(&(NOTES[i].duration), 4, 1, f);
	}
	fclose(f);
	printf("Song write complete. File is %s", fname);

	printf("\n");
	return 0;
}

/** TODO intentar pasar a este formato. notas sin el *55
note NOTES[] = {
		{261, 55},
		{0, 55},
		{261, 55},
		{0, 55},
		{261, 55},
		{349, 440},
		{0, 110},
		{440, 275},
		{0, 55},
		{261,55},
		{0,55},
		{261,55},
		{0,55},
		{261,55},
		{349, 440},
		{0, 110},
		{440, 275},
		{0, 825},

		{349, 165},
		{0,55},
		{349, 110},
		{329,55},
		{0,55},
		{329,55},
		{0,55},
		{293,55},
		{0,55},
		{293,55},
		{0,55},
		{261, 165},
		{0,550},

		{261,55},
		{0,55},
		{261,55},
		{0,55},
		{261,55},
		{329, 275},
		{0, 275},
		{392, 275},
		{0,55},
		{261,55},
		{0,55},
		{261,55},
		{0,55},
		{261,55},
		{329, 275},
		{0, 275},
		{392,550},
		{0,550},

		{523, 275},
		{0,55},
		{587,55},
		{0,55},
		{523,55},
		{0,55},
		{466,55},
		{0,55},
		{440,55},
		{0,55},
		{392,55},
		{0,55},
		{349, 275}
	};
	*/