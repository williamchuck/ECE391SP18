/*
 * rtc.c
 *
 *  Created on: Mar 11, 2018
 *      Author: Dennis, Linz Chiang
 */

#include "rtc.h"

/* Definitions for RTC ports and registers*/
#define RTC_DIS_NMI         0x80    //set this bit when writing to addr port to disable NMI.
#define RTC_REG_A           0x0A
#define RTC_REG_B           0x0B
#define RTC_REG_C           0x0C
#define RTC_ADDR_PORT       0x70
#define RTC_DATA_PORT       0x71
#define RTC_IRQ             8

//Definitions for RTC register settings
#define RTC_OSC_ON        0x20  //set DV2:0 to 010 to turn on oscillator
#define RTC_RS_CLEAR_MASK 0xF0  //mask to clear rate selector
#define RTC_PIE           0x40  //rtc periodic interrupt enable bit

// Constants for frequency calculation
#define MAX_FREQ            1024
#define FREQ_RATE_CALC      32768
#define RTC_DEF_FREQ        2

//flag to indicate interrupt for read function
volatile int interrupt_occured = 0;

//cp1 test interrupts in lib.c
extern void test_interrupts(void);

//change frequency helper
int32_t rtc_changeFreq(uint32_t freq);


/*  rtc_isr
 *  Interrupt service routine for RTC
 *  Side effects: display garbage on screen, clear interrupt on RTC
 */
void rtc_isr(){
    outb(RTC_REG_C, RTC_ADDR_PORT);   //select register C
    inb(RTC_DATA_PORT);          //read register to clear interrupt
	interrupt_occured = 1;
}
/*  rtc_enable_interrupt
 *  Enable RTC periodic interrupt and install the rtc_isr interrupt handler
 */
int rtc_enable_interrupt(){
    //referenced from https://wiki.osdev.org/RTC
    unsigned long flags;
    cli_and_save(flags);

    outb(RTC_DIS_NMI | RTC_REG_A, RTC_ADDR_PORT);   //select register A, disable NMI
    outb(RTC_OSC_ON, RTC_DATA_PORT);   //Turn on oscillator
    //(update rate is 0Hz, will be set by rtc_open when device opened)

    outb(RTC_DIS_NMI | RTC_REG_B, RTC_ADDR_PORT);   //select register B, disable NMI
    uint8_t regB=inb(RTC_DATA_PORT);//read current value

    outb(RTC_DIS_NMI | RTC_REG_B, RTC_ADDR_PORT);   //select register B, disable NMI
    outb(regB | RTC_PIE, RTC_DATA_PORT); //set Periodic Interrupt Enable bit and write it back

    outb(RTC_REG_B, RTC_ADDR_PORT);//just to enable NMI
    inb(RTC_DATA_PORT); //just read

    restore_flags(flags);
    return request_irq(RTC_IRQ, &rtc_isr);//request to install handler
}

/* rtc_changeFreq
 * DESCRIPTION: Change the RTC frequency
 * INPUTS: freq -- the frequency to be set
 * OUTPUTS: None
 */
int32_t rtc_changeFreq(uint32_t freq){
	uint8_t rate = 1;
	// check power of 2
	if( (freq & (freq-1)) != 0)
		return -1;
	// the freq is not allowed to be higher than 1024 or lower than 2
	if(freq < RTC_DEF_FREQ || freq > MAX_FREQ)
		return -1;
	while((FREQ_RATE_CALC>>(rate-1)) != freq){
		rate++;
	}

	unsigned long flags;
	cli_and_save(flags);
	uint8_t regA = 0x00;
	outb(RTC_DIS_NMI | RTC_REG_A, RTC_ADDR_PORT);   //select register A
	regA = inb(RTC_DATA_PORT);                      //read current value

	regA = regA & RTC_RS_CLEAR_MASK;	//clear rate selector bits
	regA = regA | rate;                 //set new rate
	outb(RTC_DIS_NMI | RTC_REG_A, RTC_ADDR_PORT);   //select register A
	outb(regA, RTC_DATA_PORT);                      //write new rate

	outb(RTC_REG_B, RTC_ADDR_PORT);//just to enable NMI
	inb(RTC_DATA_PORT); //just read

	restore_flags(flags);
	return 0;
}

/* rtc_open
 * DESCRIPTION: Set the RTC frequency to default value 2
 * INPUTS: none
 * OUTPUTS: 0
 */
int32_t rtc_open(const int8_t* fname){
	rtc_changeFreq(RTC_DEF_FREQ);
	return 0;	//success
}

/* rtc_read
 * DESCRIPTION: block for another RTC interrupt occur
 * INPUTS: none
 * OUTPUTS: 0
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
    unsigned long flags;
    cli_and_save(flags);
	// clear the flag (set interrupt occured to false)
	interrupt_occured = 0;
	restore_flags(flags);

	// wait for another RTC interrupt
	while(!interrupt_occured){
		// do nothing
	}
	return 0;
}

/* rtc_write
 * DESCRIPTION: Change the frequency
 * INPUTS: buf: A pointer
 *		   nbytes: Should be 4 (buf is a uint32_t pointer), or will not do anything
 * OUTPUTS: 0 -- Success
 *			-1 -- Fail
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
    int32_t err;
    // check error conditions
	if(buf == NULL || nbytes != sizeof(uint32_t))
		return -1;
	else{
	    //TODO: use userspace address-check after being implemented.
		uint32_t frequency = *((uint32_t*)buf);			//get frequency
		err = rtc_changeFreq(frequency);
	}
	if(err)return -1;
	return nbytes;
}

/* rtc_close
 * DESCRIPTION: does nothing
 * INPUTS: none
 * OUTPUTS: 0
 */
int32_t rtc_close(int32_t fd){
	return 0;
}
