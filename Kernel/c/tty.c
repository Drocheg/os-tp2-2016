#include <tty.h>
#include <keyboard.h>
#include <video.h>


#define STDIN_BUFFER_SIZE 1024
#define STDOUT_BUFFER_SIZE 1024
#define STDERR_BUFFER_SIZE 1024


/* Typedefs */
typedef enum {OTHER = 0, NUMBER, LETTER, SYMBOL} charTypes;



/* STDIN */
static char stdinBuffer[STDIN_BUFFER_SIZE];
static uint64_t stdinBufferSize = 0;
static uint64_t stdinEnqIdx = 0;
static uint64_t stdinDeqIdx = 0;
static uint64_t stdinHasData = 0;   /* Each time an '\n' is enqueued, stdinHasData is incremented
                                     * Each time an '\n' is dequeued, stdinHasData is decremented
                                     * If no '\n' in the queue, stdin will asume there is no data to read
                                     */ 

static int64_t stdinEnqueueChar(char character);
static int64_t stdinDequeueChar();
static int8_t stdinBufferIsEmpty(uint64_t index);


/* STDOUT */
static char stdoutBuffer[STDOUT_BUFFER_SIZE];
static uint64_t stdoutBufferSize = 0;
static uint64_t stdoutEnqIdx = 0;
static uint64_t stdoutDeqIdx = 0;

static int64_t stdoutEnqueueChar(char character);
static int64_t stdoutDequeueChar();
static int64_t stdoutPopChar();


/* STDERR */
static char stderrBuffer[STDERR_BUFFER_SIZE];
static uint64_t stderrBufferSize = 0;
static uint64_t stderrEnqIdx = 0;
static uint64_t stderrDeqIdx = 0;

static int64_t stderrEnqueueChar(int64_t value);
static int64_t stderrDequeueChar();
static int64_t stderrPopChar();




/* Misc */
static char *prompt = "$>_";
static int64_t backSpaceLimit = 0;

static void ttyKBDPrintChar(char c);
static int64_t ttySTDOUTPrint(char c);
static int64_t ttySTDERRPrint(char c);
static uint64_t analizeScanCode(uint64_t code); 




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




/***********************************************
 *            Exposed functions
 ***********************************************/



/*
 * Adds an element into stdin buffer.
 * A scancode is passed as an argument. 
 * The function gets the keyboard state to decode the scancode into a characted
 * If it doesn't correspond to a valid character, it's not enqueued
 *
 * Note: This routine is triggered by the keyboard ISR when it's on TTY mode
 */
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
    stdinEnqueueChar(character);
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





void stdoutFFlush() {
    while(!stdoutIsEmpty(0)) {
        ncPrintChar((char) stdoutDequeueChar());
    }
    backSpaceLimit = 0;
}

void stderrFFlush() {
    while(!stderrIsEmpty(0)) {
        ncPrintChar((char) stderrDequeueChar()); 
    }
    backSpaceLimit = 0;
}




/***********************************************
 *          FileManager implementations
 ***********************************************/



/* STDIN */
int8_t stdinReadChar(uint64_t index, char *dest) {
    int64_t result = stdinDequeueChar();
    if (result == -1) {
        return -1;
    }
    *dest = (char) result;
    return 1;
}
int8_t stdinWriteChar(uint64_t index, char *src) {
    return -1;  /* Unsupported operation */
}
int8_t stdinIsFull(uint64_t index) {
    return (stdinBufferSize != STDIN_BUFFER_SIZE);
}
int8_t stdinIsEmpty(uint64_t index) {
    return (stdinHasData <= 0);
}
int8_t stdinClose(uint64_t index) {
    return 0;
}

/* STDOUT */
int8_t stdoutReadChar(uint64_t index, char *dest) {
    return -1;  /* Unsupported operation */
}
int8_t stdoutWriteChar(uint64_t index, char *src) {
    return ttySTDOUTPrint(*src);
}
int8_t stdoutIsFull(uint64_t index) {
    return (stdoutBufferSize == STDOUT_BUFFER_SIZE);
}
int8_t stdoutIsEmpty(uint64_t index) {
    return (stdoutBufferSize == 0);
}
int8_t stdoutClose(uint64_t index) {
    return 0;
}

/* STDERR */
int8_t stderrReadChar(uint64_t index, char *dest) {
    return -1;  /* Unsupported operation */
}
int8_t stderrWriteChar(uint64_t index, char *src) {
    return ttySTDERRPrint(*src);
}
int8_t stderrIsFull(uint64_t index) {
    return (stderrBufferSize == STDERR_BUFFER_SIZE);
}
int8_t stderrIsEmpty(uint64_t index) {
    return (stderrBufferSize == 0);
}
int8_t stderrClose(uint64_t index) {
    return 0;
}







/***********************************************
 *          Buffers Management
 ***********************************************/


/* STDIN */
static int64_t stdinEnqueueChar(char character) {

	if (stdinIsFull(0)) {
		return -1;
	}
	stdinBuffer[stdinEnqIdx++] = character;
    stdinBufferSize++;
	if (stdinEnqIdx == STDIN_BUFFER_SIZE) {
		stdinEnqIdx = 0;
	}
    if (character == '\n') {
        stdinHasData++;
    }
    return 0;
}
static int64_t stdinDequeueChar() {

	char c = 0;
	if (stdinBufferIsEmpty(0)) {
		return -1; /* Same position of index can be achieved is full or if is empty */
	}
	c = stdinBuffer[stdinDeqIdx++];
    stdinBufferSize--;
	if (stdinDeqIdx == STDIN_BUFFER_SIZE) {
		stdinDeqIdx = 0;
	}
    if (c == '\n') {
        stdinHasData--;
    }
	return c;
}
static int8_t stdinBufferIsEmpty(uint64_t index) {
    return (stdinBufferSize == 0);
}

/* STDOUT */
static int64_t stdoutEnqueueChar(char character) {

    if (stdoutIsFull(0)) {
        return -1;
    }

    stdoutBuffer[stdoutEnqIdx++] = character;
    stdoutBufferSize++;
    if (stdoutEnqIdx == STDOUT_BUFFER_SIZE) {
        stdoutEnqIdx = 0;
    }
    return 0;
}
static int64_t stdoutDequeueChar() {

    char c = 0;
    if (stdoutIsEmpty(0)) {
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

    if (stdoutIsEmpty(0)) {
        return -1;
    }
    if (stdoutEnqIdx == 0) {
        stdoutEnqIdx = STDOUT_BUFFER_SIZE;
    }
    stdoutEnqIdx--;
    return (char) stdoutBuffer[stdoutEnqIdx];
}

/* STDERR */
static int64_t stderrEnqueueChar(int64_t value) {

    if (stderrIsFull(0)) {
        return -1;
    }

    stderrBuffer[stderrEnqIdx++] = (char) value;
    stderrBufferSize++;
    if (stderrEnqIdx == STDERR_BUFFER_SIZE) {
        stderrEnqIdx = 0;
    }
    return 0;
}
static int64_t stderrDequeueChar() {

    char c = 0;
    if (stderrIsEmpty(0)) {
        return -1;
    }
    c = stderrBuffer[stderrDeqIdx++];
    stderrBufferSize--;
    if (stderrDeqIdx == STDERR_BUFFER_SIZE) {
        stderrDeqIdx = 0;
    }
    return c;
}
static int64_t stderrPopChar() {

    if (stderrIsEmpty(0)) {
        return -1;
    }
    if (stderrEnqIdx == 0) {
        stderrEnqIdx = STDERR_BUFFER_SIZE;
    }
    stderrEnqIdx--;
    return (char) stderrBuffer[stderrEnqIdx];
}






/***********************************************
 *              Misc Functions
 ***********************************************/


/*
 * Analyzes a scancode and returns which type it is
 */
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

/*
 * Prints a character when added into stdin buffer only if that character must be printed
 */
static void ttyKBDPrintChar(char c) {

    switch (c){

        case '\n':
            ncPrintChar(c);
            // ncPrintChar('_');
            backSpaceLimit = 0;
            break;

        case '\b':
            if (backSpaceLimit > 0) {
                backSpaceLimit--;
                ncPrintChar(c);
                // ncPrintChar('_');
            }
            break;
        default:
            backSpaceLimit++;
            ncPrintChar(c);
            // ncPrintChar('_');
    }
}

/*
 * Prints a character into STDOUT
 * If the character is '\n', an stdoutFFlush is executed
 */
static int64_t ttySTDOUTPrint(char c) {

    int64_t result = 0;
    if (c == '\b') {
        result = stdoutPopChar();
    } else {
        result = stdoutEnqueueChar(c);
    }
    if (c == '\n') {
        stdoutFFlush();
    }
    return result;
}

/*
 * Prints a character into STDERR
 * If the character is '\n', an stdoutFFlush is executed
 */
static int64_t ttySTDERRPrint(char c) {

    int64_t result = 0;
    if (c == '\b') {
        result = stderrPopChar(); 
    } else {
        result = stderrEnqueueChar((int64_t) c); 
    }
    if (c == '\n') {
        stderrFFlush();
    }
    return result;
}


