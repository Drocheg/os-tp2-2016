#ifndef PIT_H
#define PIT_H

#include <stdint.h>

/**
* Sets the PIT frequency. Recommend not changing it once set.
*
* @param hz The frequency, in hertz.
*/
void setPITfrequency(uint16_t hz);

/**
* @return The currently set PIT frequency, or 0 if it has not
* been set and is at default.
*/
uint16_t getPITfrequency();

/**
* Increments the tick count since boot. Only interrupt handler
* should call this.
*
* @return The ticks elapsed since boot.
*/
uint64_t tick();

/**
* @return The current time, in milliseconds, since boot.
*/
uint64_t time();

/**
* @return The current tick count since boot.
*/
uint64_t ticks();

#endif