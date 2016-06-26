#include <pit.h>
#include <libasm.h>

#define PORT_0 0x40
#define PORT_1 0x41
#define PORT_2 0x42
#define COMMAND_PORT 0x43

static uint16_t freq = 0;

//Credit goes to http://www.osdever.net/bkerndev/Docs/pit.htm
void setPITfrequency(uint16_t hz)
{
    int divisor = 1193180 / hz;		/* Calculate our divisor */
    //Set interrupt frequency
    outb(COMMAND_PORT, 0x36);		/* Set our command byte 0x36 */
    outb(PORT_0, divisor & 0xFF);	/* Set low byte of divisor */
    outb(PORT_0, divisor >> 8);		/* Set high byte of divisor */
	freq = hz;
}

uint16_t getPITfrequency() {
	return freq;
}