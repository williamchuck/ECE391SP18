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

volatile int rtc_interrupt_occured[3] = {1,1,1};

/* initialize_RTC
 *
 * Description: Initialize the RTC
 * Inputs: None
 * Outputs: None
 * Side Effects: As description
 */
void initialize_RTC(){

	uint8_t regB = 0x00;//Register B of RTC
	outb(RTC_REG_B, RTC_PORT);
	regB = inb(COMS_PORT);
	regB = regB | 0x40;//This turns on bit 6
	outb(RTC_REG_B, RTC_PORT);
	outb(regB, COMS_PORT);

	enable_irq(2);
	enable_irq(8);
	outb(RTC_REG_C, RTC_PORT);
	inb(COMS_PORT);
}

/* changeFreq_RTC
 *
 * Description: Change the rtc frequency
 * Inputs: freq: The frequency that we want to change into
 * Outputs: None
 * Side Effects: As description
 */
void changeFreq_RTC(uint32_t freq){

	uint8_t rate = 1;

	/* if freq is over 1024, it is an error */
	if(freq>MAX_FREQ)
		return;
	while( ((FREQ_FOR_RATE_CALC)>>(rate-1)) != freq )
	{
		rate++;
		/* if freq is not the power of 2 or over 2^15, it is an error */
		if(rate>15)
			return;
	}

	uint8_t regA = 0x00;
	outb(RTC_REG_A, RTC_PORT);
	regA = inb(COMS_PORT);
	outb(RTC_REG_A, RTC_PORT);
	regA = regA & RTC_FRQ_ACT_LOW;//0xF0
	regA = regA | rate;
	outb(regA, COMS_PORT);
}

/* open_RTC
 *
 * Description: Set the freq to default value 2
 * Inputs: filename, Not used for now
 * Outputs: 0, Always success
 * Side Effects: As description
 */
int32_t open_RTC(const uint8_t* filename){
	changeFreq_RTC(RTC_DEFAULT_FREQ);
	return 0;//success
}

/* read_RTC
 *
 * Description: Holding on until another RTC interrupt occur
 * Inputs: fd: Not used for now
 *		   buf: Not used for now
 *		   nbyte: Not used for now
 * Outputs: 0, Always success
 * Side Effects: As description
 */
int32_t read_RTC(int32_t fd, void* buf, int32_t nbytes){
	rtc_interrupt_occured[get_tty()]= 1; //set it to 1, active low methodology followed everywhere
	while (rtc_interrupt_occured[get_tty()] == 1){//like a spin lock
	}
	return 0;
}

/* write_RTC
 *
 * Description: Change the frequency
 * Inputs: fd: Not used for now
 *		   buf: A pointer
 *		   nbyte: Should be 4 (NBYTE_DEFAULT_VAL), or will not do anything
 * Outputs: 0, Success; -1, Fail
 * Side Effects: As description
 */
int32_t write_RTC(int32_t fd, const void* buf, int32_t nbytes){
	if(buf == NULL || nbytes != NBYTE_DEFAULT_VAL)
		return -1;
	else{
		uint32_t frequency = *((int32_t*)buf);//gives the frequency
		changeFreq_RTC(frequency);
	}
	return nbytes;
}

/* close_RTC
 *
 * Description: Set the freq back to default value 2
 * Inputs: fd: Not used for now
 * Outputs: 0, Always success
 * Side Effects: As description
 */
int32_t close_RTC(int32_t fd){
	return 0;//success
}

/* RTC_handler
 *
 * Description: Only changes the RTC flag when RTC interrupt occurs
 * Inputs: None
 * Outputs: None
 * Side Effects: As description
 */
void RTC_handler(){
	int8_t garbage;
	rtc_interrupt_occured[0] = 0;
	rtc_interrupt_occured[1] = 0;
	rtc_interrupt_occured[2] = 0;
	outb(RTC_REG_C, RTC_PORT);
    garbage = inb(COMS_PORT);
    send_eoi(RTC_EOI);
}
