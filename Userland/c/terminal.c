#include <usrlib.h>

// #define START(ProgramName, EntryPoint) (\
// 	static startProgramName(uint64_t argc, char *argv[]) {\
// 		exit(EntryPoint(argv, argv));\
// 	}\
// )

// START(Beep, beep)

#ifndef COMMAND_MAX_LENGTH
#define COMMAND_MAX_LENGTH 256
#endif

typedef uint64_t (*EntryPointFn)(uint64_t argc, char* argv[]);

struct command_s {
	char *name; 
	EntryPointFn entryPoint;
	char *help;
};


static struct command_s commandsList[] = {
	
	{"help", help, "Shows this help"},
	
	{"hello", sayHello, "Greets the user"},
	
	{"clear", clearScreen, "Clears the screen"},
	
	{"exit", exitKernel, "Exits the kernel"},

	{"beep", beep, "Makes a beep using the PC speaker"},
	
	{"playsong", playMainSong, "Plays the first song loaded in the data module"},
	
	{"playsong2", playSongTwo, "Plays the second song loaded in the data module"},
	
	{"jalp", jalp, "Ai can't spik inglish"},
	
	{"piano", pianoMode, "Turns your keyboard into a piano!"},
	
	{"reboot", reboot, "Reboots the system"},
	
	{"scroll", scroll, "Scrolls an extra line"},
	
	{"surpriseme", rainbow, "Surprise surprise..."},
	
	{"time", getTime, "Get ms since system boot"},
	
	{"sleep", sleepForTwoSeconds, "Sleep for about 2 seconds"},
	
	{"mq", testMQ, "Test MQs"},
	
	{"img", imgTest, "Paints an image stored at 0x500000"},
	
	{"1", bangBang, "Re-run your last valid command"},
	
	{"game", game, "Play Game"},
	
	{"ps", ps, "Print ps"},
	
	{"ipcs", ipcs, "Print ipcs"}
};

static uint64_t terminalRunning = 0;


static int64_t matchCommand(char *command) {

	int64_t i = 0;
	uint64_t commandsCount = sizeof(commandsList) / sizeof(struct command_s);
	while (i < commandsCount) {
		if (!strcmp(command, (commandsList[i]).name)) {
			return i;
		}
		i++;
	}
	return -1;
}


static int64_t runCommand(uint64_t index, uint64_t argc, char *argv[]) {

	char *programName = (commandsList[index]).name;
	EntryPointFn entryPoint = (commandsList[index]).entryPoint;
	int64_t childPid = createProcess(programName, entryPoint, argc, argv);
	if (childPid > 0) {
		waitpid(childPid);
	}
	return childPid;
}


int terminalMain(uint64_t argc, char *argv[]) {

	char command[COMMAND_MAX_LENGTH];
	terminalRunning = 1;


	while(terminalRunning) {

		getLine(command, COMMAND_MAX_LENGTH);
		int64_t index = matchCommand(command);
		if (index >= 0) {
			char *ar[] = {"Hola"};
			runCommand(index, 1, ar);
		}





	}






}



/*
 * Gets a line (i.e. a string finishing in '\n') from STDIN
 * Returns the string length
 *
 * Note: it doesn't include the '\n' character in the string, but it null-terminates it
 */
static int64_t getLine(char *buffer, uint64_t buffSize){

	uint64_t i = 0;
	char c;

	while ( ((c = getchar()) != '\n') && (i < (buffSize - 1)) ) {
		buffer[i++] = c;
	}
	buffer[i] = 0; /* NULL-terminates the string */

	return (i - 1);
}




