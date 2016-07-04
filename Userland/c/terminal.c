#include <terminal.h>
#include <stdlib.h>
#include <usrlib.h>
#include <stdio.h>
#include <piano.h>
#include <unistd.h>
#include <game.h>
#include <file-common.h>
#include <mq.h>


static int64_t stringToInt(char *str, int64_t *result); /* TODO move this */


#ifndef COMMAND_MAX_LENGTH
#define COMMAND_MAX_LENGTH 256
#endif

/* Terminal included programs entry points */
static void help(uint64_t argc, char *argv[]);
static void sayHello(uint64_t argc, char *argv[]);
static void poweroff(uint64_t argc, char *argv[]);
static void beep(uint64_t argc, char *argv[]);
static void clear(uint64_t argc, char *argv[]);
static void piano(uint64_t argc, char *argv[]);
static void reboot(uint64_t argc, char *argv[]);
static void scroll(uint64_t argc, char *argv[]);
static void getTime(uint64_t argc, char *argv[]);
static void sleep(uint64_t argc, char *argv[]);
static void testMQ(uint64_t argc, char *argv[]);
void game(uint64_t argc, char *argv[]);
void ps(uint64_t argc, char *argv[]);
void ipcs(uint64_t argc, char *argv[]);



/* Terminal included programs main functions */
static void help_main();
static void sayHello_main();
static void poweroff_main();
static void beep_main();
static void reboot_main();
static void scroll_main();
static void getTime_main();
static void sleep_main(uint64_t argc, char *argv[]);
static void testMQ_main();
static void ps_main();
static void ipcs_main();

typedef void (*EntryPointFn)(uint64_t argc, char* argv[]);

struct command_s {
	char *name; 
	EntryPointFn entryPoint;
	char *help;
};


static struct command_s commandsList[] = {
	
	{"help", help, "Shows this help"}, // DONE
	
	{"hello", sayHello, "Greets the user"}, // DONE
	
	{"clear", clear, "Clears the screen"}, // DONE
	
	{"poweroff", poweroff, "Exits the kernel"}, // DONE

	{"beep", beep, "Makes a beep using the PC speaker"}, // DONE
	
	{"piano", piano, "Turns your keyboard into a piano!"}, // DONE
	
	{"reboot", reboot, "Reboots the system"}, // DONE
	
	{"scroll", scroll, "Scrolls an extra line"}, // DONE
	
	// {"surpriseme", rainbow, "Surprise surprise..."},
	
	{"time", getTime, "Get ms since system boot"}, // DONE
	
	{"sleep", sleep, "Sleep for about 2 seconds"}, // DONE
	
	{"mq", testMQ, "Test MQs"}, // DONE
	
	// {"1", bangBang, "Re-run your last valid command"},
	
	{"game", game, "Play Game"}, // DONE
	
	{"ps", ps, "Print ps"}, // DONE
	
	{"ipcs", ipcs, "Print ipcs"} // DONE
};

static uint64_t terminalRunning = 0;
// static char lastCommand[COMMAND_MAX_LENGTH] = {0};


static int64_t countCommands() {
	return sizeof(commandsList) / sizeof(struct command_s);
}

static int64_t matchCommand(char *command) {
	uint64_t commandsCount = countCommands();
	for(int64_t i = 0; i < commandsCount; i++) {
		if (streql(command, (commandsList[i]).name)) {
			return i;
		}
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


uint64_t terminalMain(uint64_t argc, char *argv[]) {

	char command[COMMAND_MAX_LENGTH];
	terminalRunning = 1;
	print("Terminal 2.0");
	print("\nTo see available commands, type help\n");

	while(terminalRunning) {
		print("$>");
		stdoutFFlush();
		readLine(command, COMMAND_MAX_LENGTH);
		int64_t index = matchCommand(command);
		if (index >= 0) {
			char *ar[] = {"Hola"};
			runCommand(index, 1, ar);
		} else {
			print("'");
			print(command);
			print("' is not a valid command\n");
		}

	}
	return 0;
}











/***********************************************
 *	 Terminal included programs entry points
 ***********************************************/
static void help(uint64_t argc, char *argv[]) {
	help_main();
	exit(0);
}

static void sayHello(uint64_t argc, char *argv[]) {
	sayHello_main();
	exit(0);
}

static void poweroff(uint64_t argc, char *argv[]) {
	poweroff_main();
	exit(0);
}

static void beep(uint64_t argc, char *argv[]) {
	beep_main();
	exit(0);
}

static void clear(uint64_t argc, char *argv[]) {
	clearScreen();
	exit(0);
}

static void piano(uint64_t argc, char *argv[]) {
	piano_main();
	exit(0);	
}

static void reboot(uint64_t argc, char *argv[]) {
	reboot_main();
	exit(0);	
}

static void scroll(uint64_t argc, char *argv[]) {
	scroll_main();
	exit(0);	
}

static void getTime(uint64_t argc, char *argv[]) {
	getTime_main();
	exit(0);
}

static void sleep(uint64_t argc, char *argv[]) {
	sleep_main(argc, argv);
	exit(0);
}
static void testMQ(uint64_t argc, char *argv[]) {
	testMQ_main();
	exit(0);
}

void game(uint64_t argc, char *argv[]) {
	char *argvAux[] = {"2"};
	argv = argvAux;
	argc = 1;
	game_main(argc, argv);
	exit(0);
}

void ps(uint64_t argc, char *argv[]) {
	ps_main();
	exit(0);
}

void ipcs(uint64_t argc, char *argv[]) {
	ipcs_main();
	exit(0);
}







/**************************************
 *	 Terminal included programs mains
 *************************************/

 static void help_main() {

	uint64_t i = 0;
	uint64_t commandsCount = countCommands();
	clearScreen();
	print("Available commands:\n");

	while (i < commandsCount) {
		print("    ");
		print((commandsList[i]).name);
		print(" - ");
		print((commandsList[i]).help);
		print("\n");
		i++;
	}
}

static void sayHello_main() {
	print("\nHello!\n");
}

static void poweroff_main() {
	terminalRunning = 0;
} 

static void beep_main() {
	soundFX(1000);
}

static void reboot_main() {
	reboot_sys();
}

static void scroll_main(){
	scroll_sys();
}

static void getTime_main() {
	print("Current time is: ");
	printNum(time());
	print("ms\n");
}

static void sleep_main(uint64_t argc, char *argv[]) {

	uint64_t miliseconds = 0;
	if (argc < 2 || stringToInt(argv[1], (int64_t *) &miliseconds)) {
		print("Usage: sleep <seconds>\n");
	}
	miliseconds *= 1000;
	sleep_sys(miliseconds);
}


static void testMQ_main() {
	char uniqName[MAX_NAME + 1] = {0};
	if(MQuniq(uniqName) == -1) {
		print("\nCouldn't open a unique MQ\n");
		return;
	}
	uint64_t fd = MQopen(uniqName, F_WRITE | F_NOBLOCK);
	if(fd == -1) {
		print("\nCouldn't open MQ ");
		print(uniqName);
		print("\n");
		return;
	}
	int64_t i;
	for(i = 0; i < time() % 420; i++) {
		MQsend(fd, "GET REKT M8 U F0KING W0T?!?!1!!1?!?!", 37);
	}
	print("Wrote \"GET REKT M8 U F0KING W0T?!?!1!!1?!?!\" ");
	printNum(i);
	print(" times in ");
	print(uniqName);
	print("\n");
}

static void ps_main() {
	ps_sys();
}

static void ipcs_main() {
	ipcs_sys();
}





/* TODO move this */
static int64_t stringToInt(char *str, int64_t *result) {

	uint64_t index = 0;
	int64_t sign = 1;
	if (str[index] == '-') {
		sign *= -1;
		index++;
	}

	while (str[index] != 0) {
		if (str[index] < '0' || str[index] > '9') {
			return -1;
		}
		(*result) = ((*result) * 10) + (str[index] - '0');
		index++;
	}
	(*result) *= sign;
	return 0;
}





