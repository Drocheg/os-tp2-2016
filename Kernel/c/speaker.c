#include <speaker.h>
#include <libasm.h>

#include <video.h>

static uint32_t soundQueue[SOUND_BUFF_SIZE] = {0};
static uint32_t soundTimeQueue[SOUND_BUFF_SIZE] = {0};
static int soundRIndex = 0, soundWIndex = 0;

//@Deprecated
uint8_t noSound() {

	return soundRIndex == soundWIndex ;
}

//@Deprecated
uint8_t soundBufferIsFull() {

	return soundRIndex == soundWIndex+1 || (soundRIndex == 0 && soundWIndex == SOUND_BUFF_SIZE-1);
}

/**
@Deprecated
Encola en el buffer de sonido, en caso de haber espacio, el sonido con su tiempo correspondiente. 
*/
uint8_t offerSound(uint32_t sound, uint32_t time) {
	if(soundBufferIsFull()) {
		ncPrint("sFull");
		return 0;
	}
	soundTimeQueue[soundWIndex] = time+1;
	soundQueue[soundWIndex++] = sound;
	if(soundWIndex == SOUND_BUFF_SIZE) {
		soundWIndex = 0;
	}
	return 1;
}

/**
@Deprecated
Reproduce la cancion si el buffer no esta vacio y le baja en 1 al de tiempo correspondiente.
Avanza en el buffer en caso de llegar el tiempo a 0. 
*/

void decreaseTimer() {
	if(noSound()) return;
	playSound(soundQueue[soundRIndex]);
	soundTimeQueue[soundRIndex]--;
	if(soundTimeQueue[soundRIndex]<=0){
		soundRIndex++;
		if(soundRIndex == SOUND_BUFF_SIZE) {
			soundRIndex = 0;
		}
	}
}

/**
Le pone un nuevo "countdown" al puerto 0x42 para que el speaker reproduzca un sonido distinto y
pone los ultimos 2 bits en 1 para que se reproduzca o simplemente los pone en 0 si recibio frecuencia 0.
Ver http://wiki.osdev.org/PC_Speaker para más informacion. 
*/
void playSound(uint32_t nFrequence) { 
	uint32_t Div;
	uint8_t tmp;
	if(nFrequence){

	    //Set the PIT to the desired frequency

		Div = 1193180 / nFrequence;
		outb(0x43, 0xb6);
		outb(0x42, (uint8_t) (Div) );
		outb(0x42, (uint8_t) (Div >> 8));

	    //And play the sound using the PC speaker
		tmp = inb(0x61);
		if (tmp != (tmp | 3)) {
			outb(0x61, tmp | 3);
		}
	}else{
		soundOff();
	}
}

//make it shutup
void soundOff() {
	uint8_t tmp = inb(0x61) & 0xFC;
	outb(0x61, tmp);
}

