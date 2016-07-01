#include <songplayer.h>
#include <usrlib.h>
#include <syscalls.h>
#include <fileDescriptors.h>
#include <interrupts.h>

/*
FORMATO del modulo:
(32 bits) cantidad de canciones
FORMATO DE CANCIONES:
(32 bits) cantidad de notas = n
	n veces:
		(32 bits) frecuencia. 
		(32 bits) tiempo

ejemplo:     cant=2             |			  cant=2             |f1              |t1      |f2              |t2      |			  cant=2              |f1              |t1      |f2              |t2
00000000000000000000000000000010 00000000000000000000000000000010 0000000100000101 00001010 0000001000001011 00001010 00000000000000000000000000000010 0000000100000101 00001010 0000001000001011 00001010
*/

/**
Encola en el buffer de sonido, mediante int80, los sonidos con sus tiempos de la cancion
cargada en el modulo de datos. 
*/

void playSong(uint32_t songNum) {

	char *songData;
	_int80(OPENDATAMODULE, &songData, 0, 0);
	
	int32_t songMaxNum = (int32_t) *songData;
	songData = songData+4;
	
	
	if(songMaxNum<=songNum) return; //Error

	for(uint32_t i=0; i<songNum; i++){
		int32_t n = (int32_t) *songData;
		songData = songData+4;	
		songData = songData+8*n;	
	}
	int32_t n = (int32_t) *songData;
	
	songData = songData+4;						//Skip bytes for n
	uint16_t freq; //Porque 16???
	uint32_t time;
	
	//clearScreen();
	//print("                   I shall now play you the song of my people\n");
	char * songDataAux = songData;
	int32_t m = n;
	//while(1){
		while(n > 0) {
			freq = *((uint32_t *)songData);
			songData = songData+4;
			time = (*((uint32_t *)songData));  
			songData = songData+4;
		  	_int80(SPEAKER, freq, 0, 0);
		  	n--;
		  	sleep(time);
		}
		n=m;
		songData = songDataAux ;
		_int80(SPEAKER, 0, 0, 0);	
	//}
	
}

void playSong_main(){
	playSong(1);

	exit(0);
	while(1);
}