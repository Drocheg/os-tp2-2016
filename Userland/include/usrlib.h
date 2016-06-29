#ifndef USRLIB_H
#define USRLIB_H

#include <stdint.h>
#include <vargs.h>
#include <processCommon.h>
#include <video-common.h>

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

uint64_t createProcess(uint64_t parentPid, char name[MAX_NAME_LENGTH], void *entryPoint, uint64_t argc, char *argv[]);

uint64_t time();

void soundFX(uint32_t freq);


void sleep(uint64_t sleepTime);

//Paints a white pixel at the specified location.
void paintPx(uint64_t x, uint64_t y);

/**
* Paints a pixel at the specified location in the specified color.
* 
* @param color A hex color (e.g. 0xFF0000 is red)
* @see RGBtoHex() in Common/include/video-common.h
*/
void paintColorPx(uint64_t x, uint64_t y, uint32_t color);

/**
* Paints the outline of the specified rectangle.
* Example:
* REKTangle myREKT = {0, 0, 100, 50, 0x00FF00};
* paintRect(&myREKT);
* Will paint a blue rectangle, 100px wide and 50px tall.
* 
* @param rekt The rectangle to paint.
* @see REKTangle in Common/include/video-common.h
*/
void paintRect(REKTangle *rekt);

//Fills the specified rectangle.
void fillRect(REKTangle *rekt);


// void paintImg(Image img);	Not done yet


#endif