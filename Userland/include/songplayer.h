#ifndef SONGPLAYER_H
#define SONGPLAYER_H
#include <stdint.h>


/**
 *Start function for the songPlayer. Arg 0 is msgQSend, the msgQ where
 *this process will send any feedback. Arg 1 is msgQReceive, the msgQ where 
 *the songPlayer will receive the orders.
 *if received a -1, this process selfdestruct.
 *if received a 0, this process song will stop.
 *if received a song number, the song will change to that song.
 *
*/
void playSong_start();//TODO usar msgQ

#endif