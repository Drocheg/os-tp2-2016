


#define STATE_GROUND 0;
#define STATE_JUMPING 1;
#define STATE_AIR 2;
#define SPACE_EMPTY 0;
#define SPACE_OBSTACLE 1;
#define HORIZONTAL_SIZE 10;
#define VERTICAL_SIZE 10;
#define GAME_TICK 50;
#define JUMP_LAG 50;
#define OBSTACLE_LAG_MULTIPLIER 6;


static uint64_t state;
static uint64_t jumpForce;
static uint64_t jumpStartTime;
static uint64_t lastObstacleUpdate;
static uint64_t lastUpdateTime;
static uint64_t posY; //Position of the center of the character
static uint64_t obstacle[HORIZONTAL_SIZE][10]={SPACE_EMPTY}; 

/*There are n vertical spaces. Character can only jump vertical. In the air he can't jump. 
If he touch an obstacle you lose. The character is 3 blocks tall, so he uses 3 of the n space 
at any given time, his center can be in any of the block from 1 to (n-2). 
Example: If Vertical_size=10, center can only be in any block from 1 to 8 and can't be in 0 or 9.
*/



void initGame(){
	state = STATE_GROUND;
	jumpForce=0;
	jumpStartTime=-1;
	lastUpdateTime=time();
	lastObstacleUpdate=time();
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
	while(1){
		update();
	//	yield();
	}
	return;
}

void jump(){
	if(state==STATE_AIR) return;
	if(jumpForce>=VERTICAL_SIZE-3) return;
	jumpForce+=1;
	if(state==STATE_GROUND){
		state=STATE_JUMPING;
		jumpStartTime=time();
	}
	return;
}

void update(){
	uint64_t updateTime=time(); //Make all updates with the same time.

	//Character Update
	if(STATE_JUMPING){
		if(jumpStartTime+JUMP_LAG>updateTime){
			state=STATE_AIR;
		}
		if(isPlayerJumping()){
			jump();
		}
	}
	if(STATE_AIR){
		uint64_t timeFromJump = updateTime-(jumpStartTime+JUMP_LAG);
		uint64_t gameTicksFromJump = timeFromJump/GAME_TICK;
		if(gameTicksFromJump=<jumpForce){
			posY=1+gameTicksFromJump;
		}else{
			posY=(1+jumpForce)-(gameTicksFromJump-jumpForce); //the max height from jump - fall
			if(posY<=1){
				posY=1;
				state=STATE_GROUND;	
			} 
		}
	}

	//Obstacle Update
	uint64_t obstacleTicksFromLastUpdate = ((updateTime-lastObstacleTime)/(GAME_TICK*OBSTACLE_LAG_MULTIPLIER)%HORIZONTAL_SIZE);
	if(obstacleTicksFromLastUpdate>0 && obstacleTicksFromLastUpdate<HORIZONTAL_SIZE){
		uint64_t newObstaclePosY = lastUpdateTime%VERTICAL_SIZE;
		for(uint64_t i=0; i<HORIZONTAL_SIZE; i++){
			for(uint64_t j =0; j<VERTICAL_SIZE; j++){
				uint64_t oldI = i+obstacleTicksFromLastUpdate;
				if(oldI>HORIZONTAL_SIZE){
					if(j=newObstaclePosY) obstacle[i][j]=SPACE_OBSTACLE;
					else obstacle[i][j]=SPACE_EMPTY;	
				}else{
					obstacle[i][j]=obstacle[oldI][j];
				}
			}
		}
	}


	lastUpdateTime=updateTime;
	return;
}

//TODO use msgQ
char isPlayerJumping(){
	
	char c = getchar();
	if(c=='w') return 1;
	else return 0;

}



