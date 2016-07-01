
int64_t inputReceiver_main(int argc, char* argv[]);


void inputReceiver_start(int argc, char* argv[]){
	int64_t result;
	result = inputReceiver_main( argc, argv);
	exit(result);
}


int64_t inputReceiver_main(int argc, char* argv[]){
	MsgQ msgQ = (MsgQ) argv[0];
}
