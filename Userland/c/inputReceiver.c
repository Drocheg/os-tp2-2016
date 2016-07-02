
#include <inputReceiver.h>
#include <stdlib.h>
#include <usrlib.h>
#include <scanCodes.h>

typedef int64_t MsgQ;

MsgQ msgQSend;
MsgQ msgQReceive; 

//TODO cambiar input a int8_t en todos lados? ScanCodes siempre eso?

//TODO borrar y usar las del merca y Juan
int64_t getScanCode(){} 
int64_t read(MsgQ msgQReceive){}
void send(int64_t input, MsgQ msgQSend){}

int64_t inputReceiver_main(int argc, char* argv[]);
int8_t checkEnd();
void processInput(int64_t * input);
int8_t validInput(int64_t inputProcessed);
void sendInput(int64_t inputProcessed);

void inputReceiver_start(int argc, char* argv[]){
	int64_t result;
	result = inputReceiver_main( argc, argv);
	exit(result);
}


int64_t inputReceiver_main(int argc, char* argv[]){
	msgQSend = (MsgQ) argv[0];
	msgQReceive = (MsgQ) argv[1];
	int64_t input;
	while(checkEnd()){
		input=getScanCode();
		processInput(&input);
		if(validInput(input)) sendInput(input);
	}
}

int8_t checkEnd(){
	int64_t msgR = read(msgQReceive);
	if(msgR==0) return 0;
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
	return inputProcessed!=0; //0 era true o false? Se puede resumir esto. 
}

void sendInput(int64_t inputProcessed){
	send(inputProcessed,msgQSend);
}