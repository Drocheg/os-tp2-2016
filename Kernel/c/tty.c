#include <tty.h>
#include <stdint.h>
#include <keyboard.h>
#include <video.h>


#define STDIN_BUFFER_SIZE 1024
#define STDOUT_BUFFER_SIZE 1024


/* Typedefs */
typedef enum {OTHER = 0, NUMBER, LETTER, SYMBOL} charTypes;


/* Misc */
static char *prompt = "$>_";
static backSpaceLimit = 0;
static void ttyKBDPrintChar(char c);
static int64_t ttySTDOUTPrint(char c);
static uint64_t analizeScanCode(uint64_t code); 


/* STDIN */
static char stdinBuffer[STDIN_BUFFER_SIZE];
static uint64_t stdinBufferSize = 0;
static uint64_t stdinEnqIdx = 0;
static uint64_t stdinDeqIdx = 0;
static void stdinEnqueueChar(int64_t value);
static int64_t stdinDequeueChar();
static uint64_t stdinIsFull();
static uint64_t stdinIsEmpty();




/* STDOUT */
static char stdoutBuffer[STDOUT_BUFFER_SIZE];
static uint64_t stdoutBufferSize = 0;
static uint64_t stdoutEnqIdx = 0;
static uint64_t stdoutDeqIdx = 0;





static uint64_t stdinDataAvailable = 0;





/* Character tables */
static char charTable[128] = {
    0, /* No key for scan code 0 */ 
    27, '1', '2', '3', '4', '5', '6', '7', '8',	'9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r',	't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, /* 29   - Control */
    
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,	/* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0,	/* Right shift */
    0,  /*PrtScr */
    0,	/* Alt */
    ' ',/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
    0,
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
    0,
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0	/* All other keys are undefined */
};
static char shiftedCharTable[128] = {
    0,  /* No key for scan code 0 */
    27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,  /* 29   - Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',
    0,  /* Left shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0,  /* Right shift */
    0,  /*PrtScr*/
    0,  /* Alt */
    ' ',/* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    0,
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    0,
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0  /* All other keys are undefined */
};








int64_t ttySTDINAddElement(uint8_t scanCode) {

	uint64_t flag = analizeScanCode(scanCode);
	uint64_t shift = isShifted();
	uint64_t caps = isCapsLockOn();
	uint64_t num = isNumLockOn();

	int64_t result = -1;
	char character = 0;

    switch(flag) {

        case NUMBER: {
            uint64_t number = ((shift != 0)? 1 : 0) - num; /* XOR operation */
            if (!number) {
                character = shiftedCharTable[scanCode];
            } else {
                character = charTable[scanCode];
            }
            result = 0;
            break;
        }
        case LETTER: {
            uint64_t maysuc = ((shift != 0)? 1 : 0) - caps; /* XOR operation */
            if (maysuc) {
                character = shiftedCharTable[scanCode];
            } else {
                character = charTable[scanCode];
            }
            result = 0;
            break;
        }
        case SYMBOL: {
            if (shift) {
                character = shiftedCharTable[scanCode];
            } else {
                character = charTable[scanCode];
            }
            result = 0;
            break;
        }
    }

    if (result) {
    	return -1;
    }
    stdinEnqueueChar((char) result);
    ttyKBDPrintChar(character);
    return (int64_t) character;
}


void setPrompt(char *newPrompt) {
    prompt = newPrompt;
}

void ttyPrintPrompt() {

    ncPrint(prompt);
    backSpaceLimit = 0;
}



void fflush() {

    while(!stdoutIsEmpty()) {
        ncPrintChar((char) stdoutDequeueChar);
    }
    backSpaceLimit = 0;
}




int64_t stdoutReadChar(uint32_t index, char *character) {
    return -1;
}


int64_t stdoutWriteChar(uint32_t index, char *character) {
    ttySTDOUTPrint(&c);
}





static uint64_t stdinIsFull() {
    return (stdinBufferSize == STDIN_BUFFER_SIZE);
}

static uint64_t stdinIsEmpty() {
    return (stdinBufferSize == 0);
}


static int64_t stdinEnqueueChar(int64_t value) {

	if (stdinIsFull()) {
		return -1;
	}

	stdinBuffer[stdinEnqIdx++] = (char) value;
    stdinBufferSize++;
	if (stdinEnqIdx == STDIN_BUFFER_SIZE) {
		stdinEnqIdx = 0;
	}
    return 0;
}

static int64_t stdinDequeueChar() {

	char c = 0;
	if (stdinIsEmpty()) {
		return -1; /* Same position of index can be achieved is full or if is empty */
	}
	c = stdinBuffer[stdinDeqIdx++];
    stdinBufferSize--;
	if (stdinDeqIdx == STDIN_BUFFER_SIZE) {
		stdinDeqIdx = 0;
	}
	return c;
}





static uint64_t stdoutIsFull() {
    return (stdoutBufferSize == STDOUT_BUFFER_SIZE);
}

static uint64_t stdoutIsEmpty() {
    return (stdoutBufferSize == 0);
}


static int64_t stdoutEnqueueChar(int64_t value) {

    if (stdoutIsFull()) {
        return -1;
    }

    stdoutBuffer[stdoutEnqIdx++] = (char) value;
    stdoutBufferSize++;
    if (stdoutEnqIdx == STDOUT_BUFFER_SIZE) {
        stdoutEnqIdx = 0;
    }
    return 0;
}


static int64_t stdoutDequeueChar() {

    char c = 0;
    if (stdoutIsEmpty()) {
        return -1;
    }
    c = stdoutBuffer[stdoutDeqIdx++];
    stdoutBufferSize--;
    if (stdoutDeqIdx == STDOUT_BUFFER_SIZE) {
        stdoutDeqIdx = 0;
    }
    return c;
}


static int64_t stdoutPopChar() {

    char c = 0;
    if (stdoutIsEmpty()) {
        return -1;
    }
    if (stdoutEnqIdx == 0) {
        stdoutEnqIdx = STDOUT_BUFFER_SIZE;
    }
    stdoutEnqIdx--;
    return (char) stdoutBuffer[stdoutEnqIdx];
}











static void ttyKBDPrintChar(char c) {

    switch (c){

        case '\n':
            ncPrintChar(c);
            ncPrintChar('_');
            backSpaceLimit = 0;
            break;

        case '\b':
            if (backSpaceLimit > 0) {
                backSpaceLimit--;
                ncPrintChar(c);
                ncPrintChar('_');
            }
            break;
        default:
            backSpaceLimit++;
            ncPrintChar(c);
            ncPrintChar('_');
    }
}

static int64_t ttySTDOUTPrint(char c) {

    int64_t result = 0;
    if (c == '\b') {
        result = stdoutPopChar();
    } else {
        result = stdoutEnqueueChar((int64_t) c);
    }
    if (c == '\n') {
        fflush();
    }
    return result;
}




static uint64_t analizeScanCode(uint64_t code) {
    
    if (code >= 2 && code <= 11) {
        return NUMBER;
    }
    if ( (code >= 16 && code <= 25) || (code >= 30 && code <= 38) || (code >= 44 && code <= 50) ) {
        return LETTER;
    }
    if (code == 1 || code == 12 || code == 13 || code == 14 || code == 15
        || code == 26 || code == 27 || code == 28 || code == 39 || code == 40
        || code == 41 || code == 43 || code == 51 || code == 52 || code == 53 || code == 57) {
        return SYMBOL;
    }
    return OTHER;
}