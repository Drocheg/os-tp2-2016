#ifndef TTY_H
#define TTY_H 

#include <stdint.h>

int64_t ttySTDINAddElement(uint8_t scanCode);
void setPrompt(char *newPrompt);
void ttyPrintPrompt();




uint64_t stdinReadChar(uint32_t index, char *character);
uint64_t stdinWriteChar(uint32_t index, char *character);
uint64_t stdinDataAvailable(uint32_t index);
uint64_t stdinHasFreeSpace(uint32_t index);


#endif /* TTY_H */