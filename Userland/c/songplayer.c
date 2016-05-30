#include <songplayer.h>
#include <usrlib.h>
#include <syscalls.h>
#include <fileDescriptors.h>
#include <interrupts.h>

/*
FORMATO DE CANCIONES:

(32 bits) cantidad de notas = n
	n veces:
		(16 bits) frecuencia
		(8 bits) tiempo

ejemplo:     cant=2             |f1              |t1       |f2              |t2
00000000000000000000000000000010 0000000100000101 00001010 0000001000001011 00001010
*/

/**
Encola en el buffer de sonido, mediante int80, los sonidos con sus tiempos de la cancion
cargada en el modulo de datos. 
*/
void playSong() {
	char *songData;
	songData = (char *) _int80(OPENDATAMODULE, 0, 0, 0);
	
	int32_t n = (int32_t) *songData;
	songData = songData+4;						//Skip bytes for n
	uint16_t freq;
	uint8_t time;
	
	clearScreen();
	print("                   I shall now play you the song of my people\n");
	while(n > 0) {
		freq = *((uint32_t *)songData);
		songData = songData+4;
		time = *((uint8_t *)songData);
		songData = songData+1;
	  	_int80(SPEAKER, freq, time, 0);
		n--;
	}
}