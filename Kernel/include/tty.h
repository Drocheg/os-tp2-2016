#ifndef TTY_H
#define TTY_H 

#include <stdint.h>

int64_t ttySTDINAddElement(uint8_t scanCode);
void setPrompt(char *newPrompt);
void ttyPrintPrompt();




int8_t stdinReadChar(uint64_t index, char *dest);
int8_t stdinWriteChar(uint64_t index, char *src);
int8_t stdinIsEmpty(uint64_t index);
int8_t stdinIsFull(uint64_t index);

int8_t stdoutReadChar(uint64_t index, char *dest);
int8_t stdoutWriteChar(uint64_t index, char *src);
int8_t stdoutIsEmpty(uint64_t index);
int8_t stdoutIsFull(uint64_t index);


#endif /* TTY_H */