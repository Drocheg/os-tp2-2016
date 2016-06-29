#include <game.h>
#include <stdlib.h>
#include <usrlib.h>
#include <scanCodes.h>
#include <syscalls.h>
#include <interrupts.h>


#define STATE_GROUND 0
#define STATE_JUMPING 1
#define STATE_AIR 2
#define SPACE_EMPTY 0
#define SPACE_OBSTACLE 1
#define HORIZONTAL_SIZE 10
#define VERTICAL_SIZE 10
#define GAME_TICK 100
#define JUMP_LAG 200
#define OBSTACLE_LAG_MULTIPLIER 6
#define NO_OBSTACLE_MULTIPLIER 5
#define GAME_OVER_LAG 10000


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
void playGame();
void jump();
void update();
uint64_t isPlayerJumping();
void gameOver();
void paintFullRect(uint64_t i,uint64_t j,uint64_t width,uint64_t height,uint32_t color);

void initGame(){
	state = STATE_GROUND;
	jumpForce=0;
	jumpStartTime=time();
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
	uint64_t loopTime=time();
	while(1){
		while(time()<loopTime+GAME_TICK/10);
		loopTime=time();
		update();
	//	yield();
	}
	return;
}

void jump(){
	if(state==STATE_AIR) return;
	if(jumpForce>=VERTICAL_SIZE-3) return;
	//print("F");
	jumpForce+=1;
	if(state==STATE_GROUND){
		//print("S");
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
		//print("J");
		if(jumpStartTime+JUMP_LAG<updateTime){
			state=STATE_AIR;
		}
	}
	if(isPlayerJumping()){
		jump();
	}
	if(state==STATE_AIR){
		print("A");
		uint64_t timeFromJump = updateTime-(jumpStartTime+JUMP_LAG);
		uint64_t gameTicksFromJump = timeFromJump/GAME_TICK;
		if(gameTicksFromJump>0){
			print("B");
			if(gameTicksFromJump<=jumpForce){
				print("C");
				posY=1+gameTicksFromJump;
			}else{
				print("D");
				if(1+jumpForce<gameTicksFromJump-jumpForce){
					posY=1;
				}else{
					posY=(1+jumpForce)-(gameTicksFromJump-jumpForce); //the max height from jump - fall
				}
			}
			if(posY>VERTICAL_SIZE-3) posY=VERTICAL_SIZE-2;
			if(posY<=1){
				print("G");
				posY=1;
				state=STATE_GROUND;	
				jumpForce=0;
			} 
		}
	}

	//TODO sacar esto y que la logica de antes ande bien.
	//if(posY>VERTICAL_SIZE-3) posY=VERTICAL_SIZE-2;
//	if(posY<=1){
//		posY=1;
//		state=STATE_GROUND;	
//		jumpForce=0;
//	} 


	//printNum(posY);

	//Obstacle Update
	uint64_t obstacleTicksFromLastUpdate = ((updateTime-lastObstacleUpdate)/(GAME_TICK*OBSTACLE_LAG_MULTIPLIER))%HORIZONTAL_SIZE;
	if(obstacleTicksFromLastUpdate>0 && obstacleTicksFromLastUpdate<HORIZONTAL_SIZE){
		uint64_t newObstaclePosY = ((updateTime/13)*7919)%(VERTICAL_SIZE*NO_OBSTACLE_MULTIPLIER);

		//TODO borrar estas 3 lineas. Debugging. 
		////printNum(newObstaclePosY);
		//print("\n");
		
		for(uint64_t i=0; i<HORIZONTAL_SIZE; i++){
			for(uint64_t j =0; j<VERTICAL_SIZE; j++){
				uint64_t oldI = i+obstacleTicksFromLastUpdate;
				if(oldI>=HORIZONTAL_SIZE){
					if(j==newObstaclePosY) obstacle[i][j]=SPACE_OBSTACLE;
					else obstacle[i][j]=SPACE_EMPTY;	
				}else{
					obstacle[i][j]=obstacle[oldI][j];

				}
				if(i==0 && (j==posY || j==posY+1 || j==posY-1)){
					if(obstacle[i][j]==SPACE_OBSTACLE){
						gameOver();
					}else{
						paintFullRect(i*50,j*50,50,50,0x00FFFF);
						
						
						//if(j==posY+1) print("O");
						//if(j==posY  ) print("I");
						//if(j==posY-1) print("N");
					}
				}else{
					if(obstacle[i][j]==SPACE_OBSTACLE){
						paintFullRect(i*50, j*50, 50, 50, 0xFF0000);
						
					}else{
						paintFullRect(i*50, j*50, 50, 50, 0x000000);	
						
					}
				}
			}
			
		}
		lastObstacleUpdate=updateTime; //TODO trabajar por bloques desde inicio
	}else{
		for(int j=0; j<VERTICAL_SIZE; j++){
			if(j==posY || j==posY+1 || j==posY-1){
				paintFullRect(0, j*50, 50, 50, 0x00FFFF);
			}else{
				paintFullRect(0, j*50, 50, 50, 0x000000);
			
			}
		}
	}
	
	for(int i=0; i<10; i++){
		paintFullRect(i*50,13*50,5,5,0x000000);
	}
	paintFullRect(jumpForce*50,13*50,5,5,0xFFFF00);




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
	clearScreen(); 
	print("\n\n\n Game Over \n rebooting");
	uint64_t loopTime=time();
	while(time()<loopTime+GAME_OVER_LAG){
		
	}
	reboot();
}

void paintFullRect(uint64_t i,uint64_t j,uint64_t width,uint64_t height,uint32_t color){
	for(int k=0;k<width;k++){
		REKTangle r = {k+i, j, 1, height, color};
		paintRect(&r);
	}
}




