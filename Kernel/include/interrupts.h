#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>


/* ASM functions (see interrupts.asm) */
void int20Receiver();
void int21Receiver();
int64_t int80Receiver(uint64_t syscallID, uint64_t p1, uint64_t p2, uint64_t p3);
void yield();

/* C functions (see interrupts.c) */
uint64_t initializeInterruptStacks();
void *getKernelStack();



#endif /* INTERRUPTS_H */