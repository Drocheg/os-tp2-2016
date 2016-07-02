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
#include <game.h>

extern char bss;
extern char endOfBinary;
static int bssCheck = 0;

static const int MAJOR_VER = 1;
static const int MINOR_VER = 1;
static int isExit = 0;
static char lastCommand[100] = {0};

typedef struct {
	char *name; 
	void (*function)(void);
	char *help;
} command;

void beep();
void exitKernel();
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
// void testMQ();



void game();


static command commands[] = {
	{"beep", beep, "Makes a beep using the PC speaker"},
	{"clear", clearScreen, "Clears the screen"},
	{"playsong", playMainSong, "Plays the first song loaded in the data module"},
	{"playsong2", playSongTwo, "Plays the second song loaded in the data module"},
	{"exit", exitKernel, "Exits the kernel"},
	{"help", help, "Shows this help"},
	{"hello", sayHello, "Greets the user"},
	{"jalp", jalp, "Ai can't spik inglish"},
	{"piano", piano, "Turns your keyboard into a piano!"},
	{"reboot", reboot, "Reboots the system"},
	{"scroll", scroll, "Scrolls an extra line"},
	{"surpriseme", rainbow, "Surprise surprise..."},
	{"time", getTime, "Get ms since system boot"},
	{"sleep", sleepForTwoSeconds, "Sleep for about 2 seconds"},
	// {"mq", testMQ, "Test MQs"},
	{"1", bangBang, "Re-run your last valid command"},
	{"game", game, "Play Game"},
	{"ps", ps, "Print ps"},
	{"ipcs", ipcs, "Print ipcs"}
};

uint64_t printProcessA();
uint64_t printProcessB();
uint64_t printProcessC();
int32_t userland_main(int argc, char* argv[]);

int32_t init_d(int argc, char* argv[]) {

	memset(&bss, 0, &endOfBinary - &bss);	//Clean BSS

	if(bssCheck != 0) {						//Improper BSS setup, abort
		return -1;
	}

	clearScreen();
	char* argvA[] = {"process A"};
	char* argvB[] = {"process B"};
//	char* argvC[] = {"process C"};
	char* argvTerminal[] = {"terminal"};
	createProcess(0, "process B", printProcessB, 1, argvB);
	createProcess(0, "process A", printProcessA, 1, argvA);
	// createProcess(0, "Terminal", userland_main, 1, argvTerminal);
	//	createProcess(0, "process C", printProcessC, 1, argvC);
	while(1);
	return 0;

}



uint64_t printProcessA() {
	//Write, blockingly, every ~10 sec. If we somehow manage to fill 8KiB, this will block until there's room
	int64_t mqFD = MQopen("test", F_WRITE | F_NOBLOCK);
	uint64_t aux = 0;
	while (1) {
		aux++;
		while(!MQisFull(mqFD)) {
			MQsend(mqFD, "12345678901234567890", 20);
		}
		print("MQ full, A taking a break");
		sleep(10000);
		// if(aux >= 3) {
		// 	if(aux == 3) {
		// 		print("\nClosing MQ returned ");
		// 		printNum(MQclose(mqFD));
		// 		print("\n");
		// 	}
		// }
		// else {
		// 	print("\nA -> 'Hello'\n");
		// 	MQsend(mqFD, "WHAZZAAAAAAAAAAA", 16);
		// }
	}
	return 0;
}

uint64_t printProcessB() {
	//Read non blockignyl ALL the time, if nothing could be read print '.' otherwise print what was read
	int64_t mqFD = MQopen("test", F_READ | F_NOBLOCK);
	print("B opened MQ");
	char buff[17] = {0};
	uint64_t aux = 0;
	while (1) {
		aux++;
			// print("\nB MQ:\n  Empty? ");
			// printNum(MQisEmpty(mqFD));
			// print("\n  Full? ");
			// printNum(MQisFull(mqFD));
		if(MQisEmpty(mqFD)) {
			print("\nMQ empty, B taking a break\n");
			sleep(1000);
		}
		int64_t bytesRead = MQreceive(mqFD, buff, 16);
		if(bytesRead > 0) {
			print("\nB <- '");
			buff[bytesRead] = 0;
			print(buff);
			print("' - ");
			printNum(bytesRead);
			print("\n");
		}
		else {
			print(".");
		}
		sleep(2000);
	}
	return 0;
}

uint64_t printProcessC() {
	print("CCCC");
	exit(0);
	return 0;
}

int32_t userland_main(int argc, char* argv[]) {
	char buffer[100];
	printVer();
	print("\nTo see available commands, type help\n");
	
	//Process input. No use of  "scanf" or anything of the sort because input is treated especially
	while(!isExit) {
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
	soundFX(1000);
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

void exitKernel() {
	isExit = 1;
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
	char* argvSongPlayer[] = {"2"};
	createProcess(0, "SongPlayer", playSong_start, 1, argvSongPlayer);
	return;
}

void playSongTwo(){
	playSong(1);
}

void game(){
	char* argvGame[] = {"2"};
	createProcess(0, "Game", game_start, 1, argvGame);
	return;
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

// void testMQ() {
// 	int64_t read = MQopen("test2", F_READ /*| F_NOBLOCK*/);
// 	print("Read FD: ");
// 	printNum(read);
// 	print("Attempting to read, should block: ");
// 	char buff[2] = {0};
// 	MQreceive(read, buff, 1);
// 	print("Read '");
// 	print(buff);
// 	print("'");
// }
