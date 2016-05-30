#ifndef INTERRUPTS_H
#define INTERRUPTS_H

void int20Receiver();

void int21Receiver();

int64_t int80Receiver(uint64_t syscallID, uint64_t p1, uint64_t p2, uint64_t p3);

#endif