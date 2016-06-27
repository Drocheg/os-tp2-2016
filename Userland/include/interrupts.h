#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

/* 
NOTE: The first 32 interrupts are Intel-reserved for exceptions, so all other
interrupts have an offset of 32.  Thus:

timer tick int 0 => int 0x20 (32)
keyboard int 1 0> int 0x21 (33)
etc.
if(int >= 32 && int <= 47) => IRQ hardware interrupt
*/

void _int20();
void _int21();
int64_t _int80(uint64_t syscallID, int64_t p1, int64_t p2, int64_t p3);

#endif