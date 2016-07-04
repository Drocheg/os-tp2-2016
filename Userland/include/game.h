#ifndef GAME_H
#define GAME_H

#include <stdint.h>


/**
 *Start function for the game. The process receive no arguments.
*/
void game_start(int argc, char* argv[]);

int64_t game_main(uint64_t argc, char* argv[]);


#endif