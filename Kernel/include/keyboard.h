#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

#define KEYBOARD_BUFF_SIZE 256

uint8_t bufferIsEmpty();

uint8_t bufferIsFull();

uint8_t pollRawKey();

uint8_t pollProcessedKey();

uint8_t offerKey(uint8_t scanCode);

#endif