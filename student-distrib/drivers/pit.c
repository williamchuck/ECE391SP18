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

void pit_isr(){
	/* Schedule next process */
	schedule();
}

void init_pit(){
	/* Install handler */
	request_irq(0, &pit_isr);
	
	/* Initialize freq */
	change_freq(50);

	/* Enable interrupt */
	enable_irq(0);
}


void change_freq(uint32_t new_freq){
	uint16_t div;

	if(new_freq <= MIN_PIT_FREQ) 
		div = MIN_DIV;
	else if(new_freq >= MAX_PIT_FREQ)
		div = MAX_DIV;
	else
		div = MAX_PIT_FREQ/new_freq;

	outb(0x34, 0x43);
	outb(div & 0x00FF, 0x40);
	outb(div >> 8, 0x40);
}
