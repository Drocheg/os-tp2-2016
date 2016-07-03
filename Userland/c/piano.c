#include <piano.h>
#include <stdlib.h>
#include <usrlib.h>
#include <scanCodes.h>
#include <syscalls.h>
#include <interrupts.h>


static int notes[][8] = {
	{16,33,65,131,262,523,1046,2093},
	{17,35,69,139,277,554,1109,2217},
	{18,37,73,147,294,587,1175,2349},
	{19,39,78,155,311,622,1244,2489},
	{21,41,82,165,330,659,1328,2637},
	{22,44,87,175,349,698,1397,2794},
	{23,46,92,185,370,740,1480,2960},
	{24,49,98,196,392,784,1568,3136},
	{26,52,04,208,415,831,1661,3322},
	{27,55,10,220,440,880,1760,3520},
	{29,58,16,233,466,932,1865,3729},
	{31,62,23,245,494,988,1975,3951}};

void offerNote(uint8_t note, uint8_t octave, uint32_t time);

/**
Funcion que entra en el "estado" piano. Reproduce sonidos precargados en una matriz,
que son 3 octavas enteras, al apretar ciertas teclas.
Espera hasta que se toque la tecla de codigo 'e' (Esc). 
*/
void piano() {
	clearScreen();
	print("                                 PIANO v1.0\n");
	print("                              Press ESC to exit");
	while(1){
		int number = getScanCode();
		if(decodeScanCode(number) == '\e') {	//Exit with escape
			break;
		}
		if(number>=16 && number<28){
			offerNote(number-16,4,2);
		}
		if(number>=30 && number<42){
			offerNote(number-30,5,2);
		}
		if(number>=44 && number<55){
			offerNote(number-44,6,2);
		}

	}
	clearScreen();
}

void offerNote(uint8_t note, uint8_t octave, uint32_t time){
  	uint32_t nFrequence = notes[note][octave];
  	_int80(SPEAKER, nFrequence, 0, 0);
  	sleep(40);
  	_int80(SPEAKER, 0, 0, 0);
}