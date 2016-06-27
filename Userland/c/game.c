

#ifndef GAME
#define STATE_GROUND 0
#define STATE_JUMPING 1
#define SPACE_EMPTY 0;
#define SPACE_OBSTACLE 1;
#define HORIZONTAL_SIZE 10;
#define VERTICAL_SIZE 10;
#endif

static uint64_t state;
static uint64_t posY; //Position of the center of the character
static uint64_t obstacle[HORIZONTAL_SIZE][10]={SPACE_EMPTY}; 

/*There are n vertical spaces. Character can only jump vertical. In the air he can't jump. If he touch an obstacle you lose.
The character is 3 blocks tall, so he uses 3 of the n space at any given time, his center can be in any of the block from 1 to (n-1). 
*/



void initGame(){
	state = STATE_GROUND;
	posY=1;
	for(uint64_t i=0; i<HORIZONTAL_SIZE; i++){
		for(uint64_t j =0; j<VERTICAL_SIZE; j++){
			obstacle[i][j]=SPACE_EMPTY;
		}
	}
	playGame();
	return;
}
void playGame(){
	return;
}