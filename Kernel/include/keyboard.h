#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <syscalls.h>



void changeMode(KeyboardMode mode);
void attendKeyboard();
uint8_t isCapsLockOn();
uint8_t isNumLockOn();
uint8_t isShifted();


#endif