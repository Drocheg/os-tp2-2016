#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>
#include <video-common.h>

//Print the specified character to screen in the specified color.
void ncPrintColorChar(char character, uint32_t color);

//Prints the scpecified char to screen. Handles special chars, wraps around screen and scrolls if necessary.
void ncPrintChar(char character);

//Prints the specified string to screen in the specified color.
void ncPrintColor(const char * string, uint32_t color);

//Prints the specified message to screen.
void ncPrint(const char * string);

//Prints the specified string to screen in the specified color, followed by a newline character.
void ncPrintlnColor(const char * string, uint32_t color);

//Prints the specified message to screen in white, followed by a newline character.
void ncPrintln(const char * string);

//Prints a newline character.
void ncNewline();

//Prints a backspace character.
void ncBackspace();

//Clears the entire screen and sets the cursor back to the beginning.
void ncClear();

//Prints the specified value in decimal notation.
void ncPrintDec(uint64_t value);

//Prints the specified value in hexadecimal notation.
void ncPrintHex(uint64_t value);

//Prints the specified value in binary notation.
void ncPrintBin(uint64_t value);

//Prints the specified value in the specified base.
void ncPrintBase(uint64_t value, uint32_t base);

//Scrolls the screen, moving all text lines one line up.
void ncScroll();

//Scrolls the specified number of lines.
void ncScrollLines(uint8_t lines);

//No doc needed.
void ncRAINBOWWWWWW();

//Enters graphic mode.
void setGraphicMode();

//Clears the entire screen.
void clear();

//Paints a pixel in the specified location (if valid) with the specified color.
void paintColorPixel(uint64_t x, uint64_t y, uint32_t color);

//Paints a white pixel in the specified location, if valid.
void paintPixel(uint64_t x, uint64_t y);

//Fills the specified rectangle.
void fillRect(REKTangle *rekt);

//Draws the specified rectangle.
void drawRect(REKTangle *rekt);

#endif