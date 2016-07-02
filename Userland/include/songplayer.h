#ifndef SONGPLAYER_H
#define SONGPLAYER_H
#include <stdint.h>

void playSong(int32_t songNum);
/**
 *Start function for the songPlayer. Arg 0 is msgQSend, the msgQ where
 *this process will send any feedback. Arg 1 is msgQReceive, the msgQ where 
 *the songPlayer will receive the signal to be destroyed and the numbers
 *of the songs to be played.
*/
void playSong_start();//TODO usar msgQ

#endif