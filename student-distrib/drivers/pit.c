#include "pit.h"
#include "../lib.h"
#include "../i8259.h"
#include "../idt/interrupt.h"
#include "../process/process.h"

#define RATE_COMMAND 0x34
#define PIT_COMMAND_PORT 0x43
#define PIT_DATA_PORT 0x40
#define MAX_PIT_FREQ 1193181
#define MIN_PIT_FREQ 18
#define MAX_DIV 1
#define MIN_DIV 65535

/*
 * pit_isr:
 * Description: Service function for pit interrupt
 * Input: None
 * Output: None
 * Effect: Schedule on every pit interrupt
 */
void pit_isr(){
	/* Schedule next process */
	schedule();
}

/*
 * init_pit:
 * Description: Initialize pit
 * Input: None
 * Output: None
 * Effect: Initialize pit for interrupt
 */
void init_pit(){
	/* Install handler */
	request_irq(0, &pit_isr);

	/* Initialize freq */
	change_freq(50);

	/* Enable interrupt */
	enable_irq(0);
}

/*
 * change_freq:
 * Description: Change frequency of pit
 * Input: New frequency
 * Output: None
 * Effect: Change frequency of pit to new frequency
 * */
void change_freq(uint32_t new_freq){
	uint16_t div;

	/* Calculate divisor based on new_freq */
	if(new_freq <= MIN_PIT_FREQ)
		div = MIN_DIV;
	else if(new_freq >= MAX_PIT_FREQ)
		div = MAX_DIV;
	else
		div = MAX_PIT_FREQ/new_freq;

	/* Send signals to pit */
	outb(RATE_COMMAND, PIT_COMMAND_PORT);
	/* Send the last 8 bits of div data */
	outb(div & 0x00FF, PIT_DATA_PORT);
	/* Send the first 8 bits of div data */
	outb(div >> 8, PIT_DATA_PORT);
}
