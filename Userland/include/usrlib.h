#ifndef USRLIB_H
#define USRLIB_H

#include <stdint.h>
#include <vargs.h>

void fread(uint8_t fd, char *buff, uint64_t maxBytes);

void fwrite(uint8_t fd, char *buff, uint64_t maxBytes);

void putchar(char c);

char getchar();

uint8_t getscancode();

void scroll();

void clearScreen();

void reboot();

void print(const char *str);

void printNum(uint64_t num);

void printf(const char *format, vargs *args);

#endif