#include <stdint.h>
#include <usrlib.h>
#include <stdlib.h>
#include <scanCodes.h>
#include <syscalls.h>
#include <interrupts.h>
#include <songplayer.h>
#include <piano.h>
#include <fileDescriptors.h>
#include <mq.h>
#include <file-common.h>

extern char bss;
extern char endOfBinary;
static int bssCheck = 0;

static const int MAJOR_VER = 1;
static const int MINOR_VER = 1;
static int EXIT = 0;
static char lastCommand[100] = {0};

typedef struct {
	char *name; 
	void (*function)(void);
	char *help;
} command;

void beep();
void exit();
void help();
void jalp();
void sayHello();
uint8_t runCommand(char *cmd);
void dumpDataModule();
void rainbow();
void * memset(void * destiny, int32_t c, uint64_t length);
void printVer();
void getTime();
void playMainSong();
void playSongTwo();
void bangBang();
void sleepForTwoSeconds();
void testMQ();

static command commands[] = {
	{"beep", beep, "Makes a beep using the PC speaker"},
	{"clear", clearScreen, "Clears the screen"},
	{"playsong", playMainSong, "Plays the first song loaded in the data module"},
	{"playsong2", playSongTwo, "Plays the second song loaded in the data module"},
	{"exit", exit, "Exits the kernel"},
	{"help", help, "Shows this help"},
	{"hello", sayHello, "Greets the user"},
	{"jalp", jalp, "Ai can't spik inglish"},
	{"piano", piano, "Turns your keyboard into a piano!"},
	{"reboot", reboot, "Reboots the system"},
	{"scroll", scroll, "Scrolls an extra line"},
	{"surpriseme", rainbow, "Surprise surprise..."},
	{"time", getTime, "Get ms since system boot"},
	{"sleep", sleepForTwoSeconds, "Sleep for about 2 seconds"},
	{"mq", testMQ, "Test MQs"},
	{"1", bangBang, "Re-run your last valid command"}
};

uint64_t printProcessA();
uint64_t printProcessB();
int32_t userland_main(int argc, char* argv[]);

int32_t init_d(int argc, char* argv[]) {

	memset(&bss, 0, &endOfBinary - &bss);	//Clean BSS

	if(bssCheck != 0) {						//Improper BSS setup, abort
		return -1;
	}

	clearScreen();
	char* argvA[] = {"process A"};
	char* argvB[] = {"process B"};
	char* argvTerminal[] = {"terminal"};
	createProcess(0, "process A", printProcessA, 1, argvA);
	// createProcess(0, "process B", printProcessB, 1, argvB);
	// createProcess(0, "Terminal", userland_main, 1, argvTerminal);

	while(1);
	return 0;

}

uint64_t printProcessA() {
	int64_t mqFD = MQopen("test", F_WRITE);
	// print("\nWrite FD for process A: ");
	// printNum(mqFD);
	// print("\nA sending 'Hello'\n");
	MQsend(mqFD, "Hello", 6);
	uint64_t aux = 0;
	while (1) {
		aux++;
		sleep(10000);
	}
	return 0;
}

uint64_t printProcessB() {
	int64_t mqFD = MQopen("test", F_READ);
	// print("\nRead FD for process B: ");
	// printNum(mqFD);
	char buff[6] = {0};
	// print("\nReceiving...");
	MQreceive(mqFD, buff, 5);
	print(buff);
	print("\n");
	uint64_t aux = 0;
	while (1) {
		aux++;
		sleep(10000);
	}
	return 0;
}

int32_t userland_main(int argc, char* argv[]) {
	char buffer[100];
	printVer();
	print("\nTo see available commands, type help\n");
	
	//Process input. No use of  "scanf" or anything of the sort because input is treated especially
	while(!EXIT) {
		uint8_t index = 0;
		uint8_t c;
		print(">_");

		while((c = getchar()) != '\n') {
			if(c != 0) {					//Recognized key, print it and save it
				if(c == '\b') {				//Entered backspace
					if(index > 0) {			//NOT at beginning of line, discard entered key
						index--;
					}
					else {
						putchar('_');		//Don't want to erase the >_ prompt, so counter the \b with another char
					}
				}
				else if(index < sizeof(buffer)-1) {	//Save key
					buffer[index++] = c;
				}
				putchar('\b');
				putchar(c);
				putchar('_');
			}
		}

		if(index > 0) {						//Don't do anything if buffer is empty
			buffer[index] = 0;				//Entry finished, terminate with null
			print("\n");
			uint8_t valid = runCommand(buffer);

			// if(!streql(buffer, "clear")) {
			// 	print("\n");
			// }
			if(!streql(buffer, "1") && valid) {
				//Save entered command
				uint8_t i;
				for(i = 0; buffer[i] != 0; i++) {
					lastCommand[i] = buffer[i];
				}
				lastCommand[i] = 0;
			}
		}
		else {
			print("\b\b");					//Empty buffer, counter next ">_" with \b\b
		}
	}
	print("\nBye-bye!");
	return 0;
}

void * memset(void * dest, int32_t c, uint64_t length) {
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)dest;

	while(length--)
		dst[length] = chr;

	return dest;
}

void printVer(const char *str) {
	print("Terminal V");
	printNum(MAJOR_VER);
	print(".");
	printNum(MINOR_VER);
}

void beep() {
	_int80(SPEAKER, 1000, 1, 0);
	sleep(50);
	_int80(SPEAKER, 0, 1, 0);
}

uint8_t runCommand(char *cmd) {
	toLowerStr(cmd);
	int found = 0;
	for(int i = 0; i < sizeof(commands)/sizeof(command); i++) {
		if(streql(cmd, commands[i].name)) {
			commands[i].function();
			found = 1;
			break;
		}
	}
	if(!found) {
		print("No such command. Try running help\n");
	}
	return found;
}

void exit() {
	EXIT = 1;
}

void sayHello() {
	print("\nHello!\n");
}

void jalp() {
	clearScreen();
	print("Iu asked for jalp frend? Jier is sam jalp:\n");
	print("Aveilabel comandz:\n");
	for(int i = 0; i < sizeof(commands)/sizeof(command); i++) {
		print("    ");
		print(commands[i].name);
		print(" (jau du iu pronaunz dat?)");
		print("\n");
	}
}

void help() {
	clearScreen();
	print("Available commands:\n");
	for(int i = 0; i < sizeof(commands)/sizeof(command); i++) {
		print("    ");
		print(commands[i].name);
		print(" - ");
		print(commands[i].help);
		print("\n");
	}
}

void rainbow() {
	_int80(RAINBOW, 0, 0, 0);
}

void getTime() {
	print("Current time: ");
	printNum(time());
	print("ms\n");
}

void playMainSong(){
	playSong(0);
}

void playSongTwo(){
	playSong(1);
}

void bangBang() {
	if(lastCommand[0] == 0) {
		print("No saved command\n");
	}
	else {
		print(lastCommand);
		print("\n");
		runCommand(lastCommand);
	}
}

void sleepForTwoSeconds() {
	print("Sleeping...");
	sleep(2000);
	print("woke up\n");
}

void testMQ() {
	int64_t read = MQopen("test", F_READ);
	print("Read FD: ");
	printNum(read);
}