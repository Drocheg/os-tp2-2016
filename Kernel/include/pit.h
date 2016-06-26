#ifndef PIT_H
#define PIT_H

#include <stdint.h>

void setPITfrequency(uint16_t hz);

uint16_t getPITfrequency();

#endif