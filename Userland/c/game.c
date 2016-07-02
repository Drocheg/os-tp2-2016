#include <game.h>
#include <stdlib.h>
#include <usrlib.h>
#include <scanCodes.h>
#include <syscalls.h>
#include <interrupts.h>
#include <songplayer.h>
#include <inputReceiver.h>
#include <mq.h>
#include <file-common.h>


#define STATE_GROUND 0
#define STATE_JUMPING 1
#define STATE_AIR 2
#define SPACE_EMPTY 0
#define SPACE_OBSTACLE 1
#define HORIZONTAL_SIZE 10
#define VERTICAL_SIZE 10
#define GAME_TICK 200
#define JUMP_LAG 400
#define OBSTACLE_LAG_MULTIPLIER 2
#define NO_OBSTACLE_MULTIPLIER 5
#define GAME_OVER_LAG 10000
#define JUMP_FX_1 21
#define JUMP_FX_2 41
#define JUMP_FX_3 82





/* Structs */
typedef struct{
	uint64_t state;
	int64_t jumpForce;
	uint64_t jumpStartTime;
	uint64_t lastObstacleUpdate;
	uint64_t lastUpdateTime;
	int64_t posY; //Position of the center of the character
	uint64_t obstacle[HORIZONTAL_SIZE][VERTICAL_SIZE]; 
	int isGameOver;
	int64_t mqFDMusicSend;
	int64_t mqFDMusicRead;
	int64_t mqFDInputReceiverSend;
	int64_t mqFDInputReceiverRead;
}gameData_t;

typedef gameData_t * GameData;

/*There are n vertical spaces. Character can only jump vertical. In the air he can't jump. 
If he touch an obstacle you lose. The character is 3 blocks tall, so he uses 3 of the n space 
at any given time, his center can be in any of the block from 1 to (n-2). 
Example: If Vertical_size=10, center can only be in any block from 1 to 8 and can't be in 0 or 9.
*/
void playGame(GameData gameData);
void jump(GameData gameData);
void update(GameData gameData);
uint64_t isPlayerJumping(GameData gameData);
void gameOver(GameData gameData);
void playJumpFX(uint64_t seed);
void paintFullRect(int64_t i,int64_t j,uint64_t width,uint64_t height,uint32_t color);
void initGame(GameData gameData);
int64_t game_main(int argc, char* argv[]);






void game_start(int argc, char* argv[]){
	int64_t result = game_main(argc, argv);
	exit(result);
	while(1); //TODO borrar el while(1);
}


int64_t game_main(int argc, char* argv[]){


	gameData_t gameData1;
	GameData gameData = &gameData1;

	//music
	gameData->mqFDMusicSend = MQopen("MQGameMusicSend", F_WRITE /*| F_NOBLOCK*/);
	char* argvSongPlayer[] = {"MQGameMusicRead", "MQGameMusicSend"};
	createProcess( "SongPlayer", playSong_start, 2, argvSongPlayer);
	int64_t songNum = 2;
	MQsend(gameData->mqFDMusicSend, (char *)&songNum, sizeof(int64_t));

	initGame(gameData);
	return 0;
}

void initGame(GameData gameData){
	clearScreen();
	gameData->state = STATE_GROUND;
	gameData->jumpForce=4;
	gameData->jumpStartTime=time();
	gameData->lastUpdateTime=time();
	gameData->lastObstacleUpdate=time();
	gameData->posY=1;
	for(int64_t i=0; i<HORIZONTAL_SIZE; i++){
		for(int64_t j =0; j<VERTICAL_SIZE; j++){
			gameData->obstacle[i][j]=SPACE_EMPTY;
		}
	}
	gameData->isGameOver = 0;
	
	gameData->mqFDInputReceiverRead = MQopen("MQGIRRead", F_READ /*| F_NOBLOCK*/);
	char* argvInputReceiver[] = {"MQGIRRead", "MQGIRSend"};
	createProcess("InputReceiver", inputReceiver_start, 1, argvInputReceiver);
	

	playGame(gameData);
	return;
}
void playGame(GameData gameData){
	uint64_t loopTime=time();
	
	while(!gameData->isGameOver){
		while(time()<loopTime+GAME_TICK/2);
		loopTime=time();
		update(gameData);
	//	yield();
	}
	return;
}

void jump(GameData gameData){
	if(gameData->state==STATE_AIR) return;
	if(gameData->jumpForce>=VERTICAL_SIZE-3) return;
	
	gameData->jumpForce+=1;
	if(gameData->state==STATE_GROUND){
		
		gameData->jumpForce=4;
		gameData->state=STATE_JUMPING;
		gameData->jumpStartTime=time();
	}
	return;
}

void update(GameData gameData){
	uint64_t updateTime=time(); //Make all updates with the same time.
	//Character Update
	if(gameData->state==STATE_JUMPING){
		
		if(gameData->jumpStartTime+JUMP_LAG<updateTime){
			playJumpFX(updateTime);
			gameData->state=STATE_AIR;

			
		}
	}
	if(isPlayerJumping(gameData)){
		jump(gameData);
	}
	if(gameData->state==STATE_AIR){
		
		uint64_t timeFromJump = updateTime-(gameData->jumpStartTime+JUMP_LAG);
		int64_t gameTicksFromJump = (timeFromJump/GAME_TICK);
		if(gameTicksFromJump>0){
			
			if(gameTicksFromJump<=gameData->jumpForce){
				
				gameData->posY=1+gameTicksFromJump;
			}else{
				
				if(1+2*gameData->jumpForce<gameTicksFromJump){//TODO usar numero que acepten negativos
					
					gameData->posY=1;
				}else{
					
					gameData->posY=1+2*gameData->jumpForce-gameTicksFromJump; //the max height from jump - fall (1+jumpForce<gameTicksFromJump-jumpForce)
				}
			}
			if(gameData->posY>VERTICAL_SIZE-3) gameData->posY=VERTICAL_SIZE-2;
			if(gameData->posY<=1){
			
				gameData->posY=1;
				gameData->state=STATE_GROUND;	
				gameData->jumpForce=0;
			} 
		}
	}

	
	//Obstacle Update
	uint64_t obstacleTicksFromLastUpdate = ((updateTime-gameData->lastObstacleUpdate)/(GAME_TICK*OBSTACLE_LAG_MULTIPLIER))%HORIZONTAL_SIZE;
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
					newState=gameData->obstacle[oldI][j];
				}
				

				if(i!=0){
					if(newState==SPACE_OBSTACLE && gameData->obstacle[i][j]==SPACE_EMPTY){
						paintFullRect(i*50+100,(VERTICAL_SIZE-j-1)*50, 50, 50, 0xFF0000);
						
					}
					if(newState==SPACE_EMPTY && gameData->obstacle[i][j]==SPACE_OBSTACLE){
						paintFullRect(i*50+100, (VERTICAL_SIZE-j-1)*50, 50, 50, 0x000000);	
						
					}
				}
				gameData->obstacle[i][j]=newState;
			
			}
			
		}
		gameData->lastObstacleUpdate=updateTime; //TODO trabajar por bloques desde inicio
	}

	for(int j=0; j<VERTICAL_SIZE; j++){
		if(j==gameData->posY || j==gameData->posY+1 || j==gameData->posY-1){
			if(gameData->obstacle[0][j]==SPACE_OBSTACLE){
				gameOver(gameData);
				return;
			}else{
				if(j==gameData->posY-1){
				
					Image * dinosaurio;
					_int80(OPENDATAIMGMODULE, &dinosaurio, 0, 0);
					paintImg(dinosaurio, 0, (VERTICAL_SIZE-(gameData->posY-1)-3)*50);
				
				}
				
				
			}
		}else{
			if(gameData->obstacle[0][j]==SPACE_OBSTACLE){
				paintFullRect(0,  (VERTICAL_SIZE-j-1)*50, 150, 50, 0xFF0000);
				
			}else{
				paintFullRect(0,  (VERTICAL_SIZE-j-1)*50, 150, 50, 0x000000);	
				
			}
		}
	}
	



	gameData->lastUpdateTime=updateTime;
	return;
}

//TODO hacer una estapa en donde checkea input y tener un variable que sea isPlayerJumping. Para el exit.
uint64_t isPlayerJumping(GameData gameData){
	
	//char c = ((gameData->lastUpdateTime/13)*7919)%2;
	int8_t isJumping = 0;
	int64_t msg=0;
	//print("isEmpty?");
	//printNum(MQisEmpty(gameData->mqFDInputReceiverRead));
	//print("\n");
	while(!MQisEmpty(gameData->mqFDInputReceiverRead)){
		MQreceive(gameData->mqFDInputReceiverRead, (char *)&msg, sizeof(int64_t));
		
		if(msg==1) isJumping=1;
	}
	
	return isJumping;

}

void gameOver(GameData gameData){
	gameData->isGameOver=1;
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

