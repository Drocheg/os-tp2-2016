#ifndef TTY_H
#define TTY_H 

#include <stdint.h>

int64_t ttySTDINAddElement(uint8_t scanCode);
void setPrompt(char *newPrompt);
void ttyPrintPrompt();




int64_t stdinReadChar(uint32_t index, char *character);
int64_t stdinWriteChar(uint32_t index, char *character);
int64_t stdinDataAvailable(uint32_t index);
int64_t stdinHasFreeSpace(uint32_t index);

int64_t stdoutReadChar(uint32_t index, char *character);
int64_t stdoutWriteChar(uint32_t index, char *character);
int64_t stdoutDataAvailable(uint32_t index);
int64_t stdoutHasFreeSpace(uint32_t index);


#endif /* TTY_H */