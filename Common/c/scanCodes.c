#include <scanCodes.h>

static char scanCodes[256] = {0};		//Unknown scan codes are mapped to 0
static uint8_t initialized = 0;

void init();

char decodeScanCode(uint8_t scanCode) {
	if(!initialized) {
		init();
	}
	if(scanCode >= sizeof(scanCodes)) return 0;
	return scanCodes[scanCode];
}

//http://wiki.osdev.org/PS/2_Keyboard#Scan_Code_Sets.2C_Scan_Codes_and_Key_Codes
//Scan code set 1
void init() {
	scanCodes[0x1E] = 'a';
	scanCodes[0x30] = 'b';
	scanCodes[0x2E] = 'c';
	scanCodes[0x20] = 'd';
	scanCodes[0x12] = 'e';
	scanCodes[0x21] = 'f';
	scanCodes[0x22] = 'g';
	scanCodes[0x23] = 'h';
	scanCodes[0x17] = 'i';
	scanCodes[0x24] = 'j';
	scanCodes[0x25] = 'k';
	scanCodes[0x26] = 'l';
	scanCodes[0x32] = 'm';
	scanCodes[0x31] = 'n';
	scanCodes[0x18] = 'o';
	scanCodes[0x19] = 'p';
	scanCodes[0x10] = 'q';
	scanCodes[0x13] = 'r';
	scanCodes[0x1F] = 's';
	scanCodes[0x14] = 't';
	scanCodes[0x16] = 'u';
	scanCodes[0x2F] = 'v';
	scanCodes[0x11] = 'w';
	scanCodes[0x2D] = 'x';
	scanCodes[0x15] = 'y';
	scanCodes[0x2C] = 'z';
	scanCodes[0x39] = ' ';
	scanCodes[0x02] = '1';
	scanCodes[0x03] = '2';
	scanCodes[0x04] = '3';
	scanCodes[0x05] = '4';
	scanCodes[0x06] = '5';
	scanCodes[0x07] = '6';
	scanCodes[0x08] = '7';
	scanCodes[0x09] = '8';
	scanCodes[0x0A] = '9';
	scanCodes[0x0B] = '0';
	scanCodes[0x0C] = '-';
	scanCodes[0x0D] = '=';
	scanCodes[0x1A] = '[';
	scanCodes[0x1B] = ']';
	scanCodes[0x2B] = '\\';
	scanCodes[0x27] = ';';
	scanCodes[0x28] = '\'';
	scanCodes[0x29] = '`';
	scanCodes[0x33] = ',';
	scanCodes[0x34] = '.';
	scanCodes[0x35] = '/';
	scanCodes[0x37] = '*';
	scanCodes[0x4A] = '-';
	scanCodes[0x4E] = '+';
	scanCodes[0x4F] = '1';
	scanCodes[0x50] = '2';
	scanCodes[0x51] = '3';
	scanCodes[0x4B] = '4';
	scanCodes[0x4C] = '5';
	scanCodes[0x4D] = '6';
	scanCodes[0x47] = '7';
	scanCodes[0x48] = '8';
	scanCodes[0x49] = '9';
	scanCodes[0x52] = '0';
	scanCodes[0x53] = '.';
	scanCodes[0x0E] = '\b';		//backspace
	scanCodes[0x1C] = '\n';		//enter
	scanCodes[0x01] = '\e';		//escape
	// scanCodes[0xE01C] = '\n';	//keypad enter
	/*scanCodes[0xE04D] = 0xE04D; //Right arrow
	scanCodes[0xE04B] = 0xE04B; //Left arrow
	scanCodes[0xE050] = 0xE050; //Down arrow
	scanCodes[0xE048] = 0xE048; //Up arrow*/
	initialized = 1;
}