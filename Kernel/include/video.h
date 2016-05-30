#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

void ncPrintChar(char character);
void ncPrintColorChar(char character, char color);
void ncPrint(const char * string);
void ncPrintColor(const char * string, char color);
void ncPrintln(const char * string);
void ncPrintlnColor(const char * string, char color);
void ncNewline();
void ncBackspace();
void ncPrintDec(uint64_t value);
void ncPrintHex(uint64_t value);
void ncPrintBin(uint64_t value);
void ncPrintBase(uint64_t value, uint32_t base);
void ncClear();
void ncScroll();
void ncScrollLines(uint8_t lines);
void ncRAINBOWWWWWW();

#endif