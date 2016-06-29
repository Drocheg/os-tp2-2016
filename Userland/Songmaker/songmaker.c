
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978


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
	
	note notesTrack1[] = {
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

	note notesTrack2[] = {
		{NOTE_C4, 55},
		{NOTE_C5, 55},
		{NOTE_A3, 55},
		{NOTE_A4, 55},
		{NOTE_AS3, 55},
		{NOTE_AS4, 55},
		{0, 330},

		{NOTE_C4, 55},
		{NOTE_C5, 55},
		{NOTE_A3, 55},
		{NOTE_A4, 55},
		{NOTE_AS3, 55},
		{NOTE_AS4, 55},
		{0, 330},


		{NOTE_F3, 55},
		{NOTE_F4, 55},
		{NOTE_D3, 55},
		{NOTE_D4, 55},
		{NOTE_DS3, 55},
		{NOTE_DS4, 55},
		{0, 330},

		{NOTE_F3, 55},
		{NOTE_F4, 55},
		{NOTE_D3, 55},
		{NOTE_D4, 55},
		{NOTE_DS3, 55},
		{NOTE_DS4, 55},
		{0, 110},

		{0, 55},
		{NOTE_DS4, 55},
		{NOTE_CS4, 55},
		{NOTE_D4, 55},
		{NOTE_CS4, 55},
		{NOTE_DS4, 55},
		{NOTE_GS3, 55},
		{NOTE_G3, 55},
	
	}

	uint32_t n = sizeof(notesTrack1) / sizeof(note);

	printf("Writing %d notes:\n", n);
	int i;
	for(i = 0; i < n; i++) {
		printf("freq = %d (0x%x) ", notesTrack1[i].freq, notesTrack1[i].freq);
		printf("duration = %d (0x%x)\n", notesTrack1[i].duration, notesTrack1[i].duration);
	}

	//Write
	uint32_t songNum = 2; //TODO cambiar magicNum
	fwrite(&songNum, 4, 1, f);
	//MainSong
	fwrite(&n, 4, 1, f);
	for(i = 0; i < n; i++) {
		fwrite(&(notesTrack1[i].freq), 4, 1, f);
		fwrite(&(notesTrack1[i].duration), 4, 1, f);
	}

	uint32_t n2 = sizeof(notesTrack2) / sizeof(note);

	printf("Writing %d notes:\n", n);
	
	for(i = 0; i < n2; i++) {
		printf("freq = %d (0x%x) ", notesTrack2[i].freq, notesTrack2[i].freq);
		printf("duration = %d (0x%x)\n", notesTrack2[i].duration, notesTrack2[i].duration);
	}

		
	//ExtraSong
	fwrite(&n2, 4, 1, f);
	for(i = 0; i < n2; i++) {
		fwrite(&(notesTrack2[i].freq), 4, 1, f);
		fwrite(&(notesTrack2[i].duration), 4, 1, f);
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