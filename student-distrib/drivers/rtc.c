/*
 * rtc.c
 *
 *  Created on: Mar 11, 2018
 *      Author: Dennis, Linz Chiang
 */

#include "rtc.h"

extern void test_interrupts(void);
/*  rtc_isr
 *  Interrupt service routine for RTC
 *  Side effects: display garbage on screen, clear interrupt on RTC
 */
void rtc_isr(){
    test_interrupts();
    outb(0x0C, 0x70);   //select register C
    inb(0x71);          //read register to clear interrupt
}
/*  rtc_enable_interrupt
 *  Enable RTC periodic interrupt and install the rtc_isr interrupt handler
 */
int rtc_enable_interrupt(){
    //referenced from https://wiki.osdev.org/RTC
    unsigned long flags;
    cli_and_save(flags);
    outb(0x8A, 0x70);   //select register A, disable NMI
    outb(0x2F, 0x71);   //Turn on oscillator, select update rate to be 500ms
    outb(0x8B, 0x70);   //select register B, disable NMI
    char regB=inb(0x71);//store current value
    outb(0x8B, 0x70);   //select register B, disable NMI
    outb(regB | 0x40, 0x71); //set Periodic Interrupt Enable bit and write it back
    outb(0x0B, 0x70);//just to enable NMI
    inb(0x71); //just read
    restore_flags(flags);
    return request_irq(8, &rtc_isr);//request to install handler
}

// flags for handling ovvured interrupts
volatile int interrupt_occured[3] = {1,1,1};

/* initialize_RTC
 * DESCRIPTION: Initialize the RTC
 * INPUTS: None
 * OUTPUTS: None
 */
void initialize_RTC(){
	uint8_t regB = 0x00;		// register B of RTC
	outb(RTC_REG_B, RTC_PORT);
	regB = inb(COMS_PORT);
	regB = regB | 0x40;			// set bit 6 to high
	outb(RTC_REG_B, RTC_PORT);
	outb(regB, COMS_PORT);
	enable_irq(2);
	enable_irq(8);
	outb(RTC_REG_C, RTC_PORT);
	inb(COMS_PORT);
}

/* changeFreq_RTC
 * DESCRIPTION: Change the RTC frequency
 * INPUTS: freq -- the frequency to be set
 * OUTPUTS: None
 */
void changeFreq_RTC(uint32_t freq){
	uint8_t rate = 1;
	// the freq is not allowed to be higher than 1024
	if(freq > MAX_FREQ)
		return;
	while((FREQ_FOR_RATE_CALC>>(rate-1)) != freq){
		rate++;
		// freq needs to be power of 2 and not larger than 2^15
		if(rate > 15)
			return;
	}
	uint8_t regA = 0x00;
	outb(RTC_REG_A, RTC_PORT);
	regA = inb(COMS_PORT);
	outb(RTC_REG_A, RTC_PORT);
	regA = regA & RTC_FRQ_ACT_LOW;		//0xF0
	regA = regA | rate;
	outb(regA, COMS_PORT);
}

/* open_RTC
 * DESCRIPTION: Set the RTC frequency to default value 2
 * INPUTS: none
 * OUTPUTS: 0
 */
int32_t open_RTC(){
	changeFreq_RTC(DEFAULT_FREQ);
	return 0;	//success
}

/* read_RTC
 * DESCRIPTION: block for another RTC interrupt occur
 * INPUTS: none
 * OUTPUTS: 0
 */
int32_t read_RTC(){
	interrupt_occured[get_tty()] = 1; 				// set to high (active low)
	while (interrupt_occured[get_tty()] == 1){		// similar to a spin lock
	}
	return 0;
}

/* write_RTC
 * DESCRIPTION: Change the frequency
 * INPUTS: fd: Not used for now
 *		   buf: A pointer
 *		   nbyte: Should be 4 (NBYTE_DEFAULT_VAL), or will not do anything
 * OUTPUTS: 0 -- Success;
  			-1 -- Fail
 */
int32_t write_RTC(int32_t fd, const void* buf, int32_t nbytes){
	if(buf == NULL || nbytes != NBYTE_DEFAULT_VAL)
		return -1;
	else{
		uint32_t frequency = *((int32_t*)buf);			//gives the frequency
		changeFreq_RTC(frequency);
	}
	return nbytes;
}

/* close_RTC
 * DESCRIPTION: does nothing
 * INPUTS: none
 * OUTPUTS: 0
 */
int32_t close_RTC(){
	return 0;
}

/* RTC_handler
 * DESCRIPTION: Changes the RTC flag when RTC interrupt occurs
 * INPUTS: none
 * OUTPUTS: None
 */
void RTC_handler(){
	int8_t garbage;
	interrupt_occured[0] = 0;
	interrupt_occured[1] = 0;
	interrupt_occured[2] = 0;
	outb(RTC_REG_C, RTC_PORT);
    garbage = inb(COMS_PORT);
    send_eoi(RTC_EOI);
}
