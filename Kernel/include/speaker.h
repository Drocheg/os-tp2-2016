#ifndef SPEAKER_H
#define SPEAKER_H

#include <stdint.h>

#define SOUND_BUFF_SIZE 256

uint8_t noSound();

uint8_t soundBufferIsFull();

uint8_t offerSound(uint32_t sound, uint32_t time);

void decreaseTimer();

void playSound(uint32_t nFrequence);

void soundOff();

void beep();

#endif