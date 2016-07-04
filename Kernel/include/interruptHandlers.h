#ifndef INTERRUPTHANDLERS_H
#define INTERRUPTHANDLERS_H

#include <stdint.h>

void IRQHandler(uint8_t irq);

uint64_t timerTickHandler(void *stack);
void keyboardHandler();
int64_t int80Handler(uint64_t syscallID, uint64_t p1, uint64_t p2, uint64_t p3);




#endif