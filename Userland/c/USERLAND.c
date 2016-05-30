#include <stdint.h>
#include <usrlib.h>
#include <stdlib.h>
#include <scanCodes.h>
#include <syscalls.h>
#include <interrupts.h>
#include <songplayer.h>
#include <piano.h>
#include <fileDescriptors.h>

extern char bss;
extern char endOfBinary;
static int bssCheck = 0;

static const int MAJOR_VER = 1;
static const int MINOR_VER = 0;
static int EXIT = 0;

typedef struct
{
	char *name; 
	void (*function)(void);
	char *help;
} command;

void beep();
void exit();
void help();
void jalp();
void sayHello();
void runCommand(char *cmd);
void dumpDataModule();
void rainbow();
void * memset(void * destiny, int32_t c, uint64_t length);
void printVer();

static command commands[] = {
	{"beep", beep, "Makes a beep using the PC speaker"},
	{"clear", clearScreen, "Clears the screen"},
	{"playsong", playSong, "Plays the song loaded as data module"},
	{"exit", exit, "Exits the kernel"},
	{"help", help, "Shows this help"},
	{"hello", sayHello, "Greets the user"},
	{"jalp", jalp, "Ai can't spik inglish"},
	{"piano", piano, "Turns your keyboard into a piano!"},
	{"reboot", reboot, "Reboots the system"},
	{"scroll", scroll, "Scrolls an extra line"},
	{"surpriseme", rainbow, "Surprise surprise..."}
};


int32_t userland_main(int argc, char *argv[]) {
	memset(&bss, 0, &endOfBinary - &bss);	//Clean BSS
	if(bssCheck != 0) {						//Improper BSS setup, abort
		return -1;
	}

	clearScreen();
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
			runCommand(buffer);
			if(!streql(buffer, "clear")) {
				print("\n");
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
}

void runCommand(char *cmd) {
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
		print("No such command. Try running help");
	}
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