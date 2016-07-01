#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

#define KEYBOARD_BUFF_SIZE 256

uint8_t bufferIsEmpty();

uint8_t bufferIsFull();

uint8_t pollRawKey();

uint8_t pollProcessedKey();

uint8_t offerKey(uint8_t scanCode);


uint64_t stdinReadChar(uint32_t index, char *character);
uint64_t stdinWriteChar(uint32_t index, char *character);
uint64_t stdinDataAvailable(uint32_t index);
uint64_t stdinHasFreeSpace(uint32_t index);

#endif