#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>


/* ASM functions (see interrupts.asm) */
void int20Receiver();
void int21Receiver();
int64_t int80Receiver(uint64_t syscallID, uint64_t p1, uint64_t p2, uint64_t p3);
uint64_t getActualStackTop();

/* C functions (see interrupts.c) */
uint64_t initializeInterruptStacks();


void *getStack(uint64_t stack);

/* TODO: Make just one function */
void *getSoftwareIntStack();
void *getTimerIntStack();
void *getKbdIntStack();



#endif /* INTERRUPTS_H */