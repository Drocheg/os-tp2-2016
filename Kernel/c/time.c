#include <time.h>
#include <libasm.h>

#define PIT_PORT_0 0x40
#define PIT_PORT_1 0x41
#define PIT_PORT_2 0x42
#define PIT_COMMAND_PORT 0x43

static uint16_t freq = 0;
static uint64_t tickCount = 0;

//Credit goes to http://www.osdever.net/bkerndev/Docs/pit.htm
void setPITfrequency(uint16_t hz)
{
    int divisor = 1193180 / hz;		/* Calculate our divisor */
    //Set interrupt frequency
    outb(PIT_COMMAND_PORT, 0x36);		/* Set our command byte 0x36 */
    outb(PIT_PORT_0, divisor & 0xFF);	/* Set low byte of divisor */
    outb(PIT_PORT_0, divisor >> 8);		/* Set high byte of divisor */
	freq = hz;
}

uint16_t getPITfrequency() {
	return freq;
}

uint64_t tick() {
	return ++tickCount;
}

uint64_t time() {
	return tickCount / (double) freq * 1000;
}

uint64_t ticks() {
	return tickCount;
}