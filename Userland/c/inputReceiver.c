
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
 

//TODO cambiar input a int8_t en todos lados? ScanCodes siempre eso?



int64_t inputReceiver_main(int argc, char* argv[]);
int8_t checkEnd(int64_t mqFDRead);
void processInput(int64_t * input);
int8_t validInput(int64_t inputProcessed);
void sendInput(int64_t inputProcessed, int64_t mqFDSend);

void inputReceiver_start(int argc, char* argv[]){
	int64_t result;
	result = inputReceiver_main( argc, argv);
	exit(result);
}


int64_t inputReceiver_main(int argc, char* argv[]){
	int64_t mqFDSend;
	int64_t mqFDRead;
	mqFDSend = MQopen( argv[0], F_WRITE /*| F_NOBLOCK*/);
	mqFDRead = MQopen(argv[1], F_READ | F_NOBLOCK);
	
	int64_t input;
	while(checkEnd(mqFDRead)){
		//input=getScanCode();
		input=((time()/13)*7919)%2;
		//processInput(&input);
		if(validInput(input)) sendInput(input, mqFDSend);
		yield();
	}
	MQclose(mqFDSend);
	MQclose(mqFDRead);
	return 0;
}

int8_t checkEnd(int64_t mqFDRead){
	
	if(!MQisEmpty(mqFDRead)){
		int64_t msg;
		MQreceive(mqFDRead, (char *)&msg, sizeof(int64_t));
		if(msg==0) return 0;
	}
	
	return 1;
}

void processInput(int64_t * input){
	char decodedInput = decodeScanCode((int8_t) *input);
	switch(decodedInput){
		case ' ':
		case 'w':
			*input=1;
			break;
		case '\e':
			*input=2;
			break;
		default:*input=0;
			break;
	}

	
}

int8_t validInput(int64_t inputProcessed){
	return inputProcessed==1; //0 era true o false? Se puede resumir esto. 
}

void sendInput(int64_t inputProcessed, int64_t mqFDSend){
	//print("isEmptySender?");
	//printNum(inputProcessed);
	MQsend(mqFDSend, (char *)&inputProcessed, sizeof(int64_t));
	//printNum(MQisEmpty(mqFDSend));
	
}