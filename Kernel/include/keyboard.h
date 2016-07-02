#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>


typedef enum {TTY = 0, RAW} KeyboardMode;


// #define KEYBOARD_BUFF_SIZE 256

// uint8_t bufferIsEmpty();

// uint8_t bufferIsFull();

// uint8_t pollRawKey();

// uint8_t pollProcessedKey();

// uint8_t offerKey(uint8_t scanCode);

// void enqueueKey();
// int64_t dequeueKey();
// int64_t peekKey();

void attendKeyboard();
uint8_t isCapsLockOn();
uint8_t isNumLockOn();
uint8_t isShifted();



#endif