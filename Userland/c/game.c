#include <game.h>
#include <stdlib.h>
#include <usrlib.h>
#include <scanCodes.h>
#include <syscalls.h>
#include <interrupts.h>
#include <songplayer.h>
#include <inputReceiver.h>


#define STATE_GROUND 0
#define STATE_JUMPING 1
#define STATE_AIR 2
#define SPACE_EMPTY 0
#define SPACE_OBSTACLE 1
#define HORIZONTAL_SIZE 10
#define VERTICAL_SIZE 10
#define GAME_TICK 100
#define JUMP_LAG 200
#define OBSTACLE_LAG_MULTIPLIER 3
#define NO_OBSTACLE_MULTIPLIER 5
#define GAME_OVER_LAG 10000
#define JUMP_FX_1 21
#define JUMP_FX_2 41
#define JUMP_FX_3 82


uint64_t state;
int64_t jumpForce;
uint64_t jumpStartTime;
uint64_t lastObstacleUpdate;
uint64_t lastUpdateTime;
int64_t posY; //Position of the center of the character
uint64_t obstacle[HORIZONTAL_SIZE][10]={SPACE_EMPTY}; 
int isGameOver = 0;

/*There are n vertical spaces. Character can only jump vertical. In the air he can't jump. 
If he touch an obstacle you lose. The character is 3 blocks tall, so he uses 3 of the n space 
at any given time, his center can be in any of the block from 1 to (n-2). 
Example: If Vertical_size=10, center can only be in any block from 1 to 8 and can't be in 0 or 9.
*/
void playGame();
void jump();
void update();
uint64_t isPlayerJumping();
void gameOver();
void playJumpFX(uint64_t seed);
void paintFullRect(int64_t i,int64_t j,uint64_t width,uint64_t height,uint32_t color);
void initGame();
int64_t game_main(int argc, char* argv[]);


void game_start(int argc, char* argv[]){
	int64_t result = game_main(argc, argv);
	exit(result);
	while(1); //TODO borrar el while(1);
}


int64_t game_main(int argc, char* argv[]){
	initGame();
	return 0;
}

void initGame(){
	clearScreen();
	state = STATE_GROUND;
	jumpForce=0;
	jumpStartTime=time();
	lastUpdateTime=time();
	lastObstacleUpdate=time();
	posY=1;
	for(int64_t i=0; i<HORIZONTAL_SIZE; i++){
		for(int64_t j =0; j<VERTICAL_SIZE; j++){
			obstacle[i][j]=SPACE_EMPTY;
		}
	}
	//music
	char* argvSongPlayer[] = {"1"};
	createProcess(0, "SongPlayer", playSong_start, 1, argvSongPlayer);
//	char* argvInputReceiver[] = {"inputReceiver"};
//	createProcess(0, "InputReceiver", inputReceiver_main, 1, argvInputReceiver);
	
	playGame();
	return;
}
void playGame(){
	uint64_t loopTime=time();
	
	while(!isGameOver){
		while(time()<loopTime+GAME_TICK/2);
		loopTime=time();
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
		
		jumpForce=1;
		state=STATE_JUMPING;
		jumpStartTime=time();
	}
	return;
}

void update(){
	uint64_t updateTime=time(); //Make all updates with the same time.
	
	//Character Update
	if(state==STATE_JUMPING){
		
		if(jumpStartTime+JUMP_LAG<updateTime){
			playJumpFX(updateTime);
			state=STATE_AIR;
		}
	}
	if(isPlayerJumping()){
		jump();
	}
	if(state==STATE_AIR){
		
		uint64_t timeFromJump = updateTime-(jumpStartTime+JUMP_LAG);
		int64_t gameTicksFromJump = (timeFromJump/GAME_TICK);
		if(gameTicksFromJump>0){
			
			if(gameTicksFromJump<=jumpForce){
				
				posY=1+gameTicksFromJump;
			}else{
				
				if(1+2*jumpForce<gameTicksFromJump){//TODO usar numero que acepten negativos
					
					posY=1;
				}else{
					
					posY=1+2*jumpForce-gameTicksFromJump; //the max height from jump - fall (1+jumpForce<gameTicksFromJump-jumpForce)
				}
			}
			if(posY>VERTICAL_SIZE-3) posY=VERTICAL_SIZE-2;
			if(posY<=1){
			
				posY=1;
				state=STATE_GROUND;	
				jumpForce=0;
			} 
		}
	}

	
	//Obstacle Update
	uint64_t obstacleTicksFromLastUpdate = ((updateTime-lastObstacleUpdate)/(GAME_TICK*OBSTACLE_LAG_MULTIPLIER))%HORIZONTAL_SIZE;
	if(obstacleTicksFromLastUpdate>0 && obstacleTicksFromLastUpdate<HORIZONTAL_SIZE){
		int64_t newObstaclePosY = ((updateTime/13)*7919)%(VERTICAL_SIZE*NO_OBSTACLE_MULTIPLIER);

	
		for(int64_t i=0; i<HORIZONTAL_SIZE; i++){
			for(int64_t j =0; j<VERTICAL_SIZE; j++){
				int64_t oldI = i+obstacleTicksFromLastUpdate;
				uint64_t newState;
				if(oldI>=HORIZONTAL_SIZE){
					if(j==newObstaclePosY) newState=SPACE_OBSTACLE;
					else newState=SPACE_EMPTY;	
				}else{
					newState=obstacle[oldI][j];
				}
				

				if(i!=0){
					if(newState==SPACE_OBSTACLE && obstacle[i][j]==SPACE_EMPTY){
						paintFullRect(i*50+100,(VERTICAL_SIZE-j-1)*50, 50, 50, 0xFF0000);
						
					}
					if(newState==SPACE_EMPTY && obstacle[i][j]==SPACE_OBSTACLE){
						paintFullRect(i*50+100, (VERTICAL_SIZE-j-1)*50, 50, 50, 0x000000);	
						
					}
				}
				obstacle[i][j]=newState;
			
			}
			
		}
		lastObstacleUpdate=updateTime; //TODO trabajar por bloques desde inicio
	}

	for(int j=0; j<VERTICAL_SIZE; j++){
		if(j==posY || j==posY+1 || j==posY-1){
			if(obstacle[0][j]==SPACE_OBSTACLE){
				gameOver();
				return;
			}else{
				if(j==posY-1){
				
					Image * dinosaurio;
					_int80(OPENDATAIMGMODULE, &dinosaurio, 0, 0);
					paintImg(dinosaurio, 0, (VERTICAL_SIZE-(posY-1)-3)*50);
				
				}
				
				
			}
		}else{
			if(obstacle[0][j]==SPACE_OBSTACLE){
				paintFullRect(0,  (VERTICAL_SIZE-j-1)*50, 150, 50, 0xFF0000);
				
			}else{
				paintFullRect(0,  (VERTICAL_SIZE-j-1)*50, 150, 50, 0x000000);	
				
			}
		}
	}
	



	lastUpdateTime=updateTime;
	return;
}

//TODO use msgQ
uint64_t isPlayerJumping(){
	
	char c = ((lastUpdateTime/13)*7919)%2;
	if(c==1) return 1;
	else return 0;

}

void gameOver(){
	isGameOver=1;
	clearScreen(); 
	print("\n\n\n Game Over ");
	
}

void paintFullRect(int64_t i,int64_t j,uint64_t width,uint64_t height,uint32_t color){
	for(int k=0;k<width;k++){
		REKTangle r = {k+i, j, 1, height, color};
		paintRect(&r);
	}
}


void playJumpFX(uint64_t seed){
	uint64_t randFX = ((seed/13)*7919)%(3);
	uint32_t jumpFX;
	switch(randFX){
		case 0:
			jumpFX=JUMP_FX_1;
			break;
		case 1:
			jumpFX=JUMP_FX_2;
			break;
		default:
			jumpFX=JUMP_FX_3;
			break;

	}

	soundFX(jumpFX);
}

