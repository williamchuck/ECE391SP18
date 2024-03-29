/*
 * rtc.c
 *  Created on: Mar 11, 2018
 *      Author: Dennis, Linz Chiang
 */

#include "rtc.h"
#include "../fs/fs.h"
#include "../process/process.h"

/* Definitions for RTC ports and registers*/
//set this bit when writing to addr port to disable NMI
#define RTC_DIS_NMI         0x80
#define RTC_REG_A           0x0A
#define RTC_REG_B           0x0B
#define RTC_REG_C           0x0C
#define RTC_ADDR_PORT       0x70
#define RTC_DATA_PORT       0x71
#define RTC_IRQ             8

//Definitions for RTC register settings
//set DV2:0 to 010 to turn on oscillator
#define RTC_OSC_ON        0x20
//mask to clear rate selector
#define RTC_RS_CLEAR_MASK 0xF0
//rtc periodic interrupt enable bit
#define RTC_PIE           0x40

// Constants for frequency calculation
#define MAX_FREQ            1024
#define MIN_FREQ			2
#define FREQ_RATE_CALC      32768
#define RTC_DEF_FREQ        1024
#define RTC_DEF_USR			2

static uint32_t proc_freq[NUM_PROC];
volatile uint32_t proc_freq_counter[NUM_PROC];
static uint32_t phys_freq = MIN_FREQ;

//flag to indicate interrupt for read function
volatile int interrupt_occured = 0;

//cp1 test interrupts in lib.c
extern void test_interrupts(void);

//change frequency helper
int32_t rtc_changeFreq(uint32_t freq);

static file_op_t rtc_file_op = {
	.open = rtc_open,
	.read = rtc_read,
	.write = rtc_write,
	.close = rtc_close
};

file_op_t* rtc_op = &rtc_file_op;

/*  rtc_isr
 *  Interrupt service routine for RTC
 *  Side effects: display garbage on screen, clear interrupt on RTC
 */
void rtc_isr(){
	//select register C
    outb(RTC_REG_C, RTC_ADDR_PORT);
	//read register to clear interrupt
    inb(RTC_DATA_PORT);
	interrupt_occured = 1;
}

/*  rtc_enable_interrupt
 *  Enable RTC periodic interrupt and install the rtc_isr interrupt handler
 */
int rtc_enable_interrupt(){
    //referenced from https://wiki.osdev.org/RTC
    unsigned long flags;
    cli_and_save(flags);

	//select register A, disable NMI
    outb(RTC_DIS_NMI | RTC_REG_A, RTC_ADDR_PORT);
	//Turn on oscillator
    outb(RTC_OSC_ON, RTC_DATA_PORT);
    //(update rate is 0Hz, will be set by rtc_open when device opened)
	//select register B, disable NMI
    outb(RTC_DIS_NMI | RTC_REG_B, RTC_ADDR_PORT);
	//read current value
    uint8_t regB=inb(RTC_DATA_PORT);
	//select register B, disable NMI
    outb(RTC_DIS_NMI | RTC_REG_B, RTC_ADDR_PORT);
	//set Periodic Interrupt Enable bit and write it back
    outb(regB | RTC_PIE, RTC_DATA_PORT);
	//to enable NMI
    outb(RTC_REG_B, RTC_ADDR_PORT);
	//read
    inb(RTC_DATA_PORT);

    restore_flags(flags);
	//request to install handler
    return request_irq(RTC_IRQ, &rtc_isr);
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
	if(freq < MIN_FREQ || freq > MAX_FREQ)
		return -1;
	while((FREQ_RATE_CALC>>(rate-1)) != freq){
		rate++;
	}

	unsigned long flags;
	cli_and_save(flags);
	uint8_t regA = 0x00;
	//select register A
	outb(RTC_DIS_NMI | RTC_REG_A, RTC_ADDR_PORT);
	//read current value
	regA = inb(RTC_DATA_PORT);
	//clear rate selector bits
	regA = regA & RTC_RS_CLEAR_MASK;
	//set new rate
	regA = regA | rate;
	//select register A
	outb(RTC_DIS_NMI | RTC_REG_A, RTC_ADDR_PORT);
	//write new rate
	outb(regA, RTC_DATA_PORT);
	//to enable NMI
	outb(RTC_REG_B, RTC_ADDR_PORT);
	//read
	inb(RTC_DATA_PORT);

	restore_flags(flags);
	return 0;
}

/* rtc_open
 * DESCRIPTION: Set the RTC frequency to default value 2
 * INPUTS: none
 * OUTPUTS: 0
 */
int32_t rtc_open(const int8_t* fname){
	/* Initialize varaibles */
	int i;
	uint32_t pid;
	dentry_t dentry;

	/* Fill in dentry */
	if(read_dentry_by_name(fname, &dentry) == -1)
		return -1;

	/* Sanity check */
	if(dentry.file_type != 0)
		return -1;

	/* Reserve file descriptor 0 and 1 for stdin and stdout, search for free entry */
	for(i = 2; i < 8; i++){
		/* If free, then fill in the current_PCB->file_desc_arr_t */
		if(current_PCB->file_desc_arr[i].flag == 0){
			current_PCB->file_desc_arr[i].f_op = &rtc_file_op;
			current_PCB->file_desc_arr[i].inode = 0;
			current_PCB->file_desc_arr[i].f_pos = 0;
			current_PCB->file_desc_arr[i].flag = 1;
			rtc_changeFreq(phys_freq);
			pid = current_PCB->pid;
			proc_freq[pid] = MIN_FREQ;
			return i;
		}
	}

	/* If current_PCB->file_desc_arr is full, return -1 */
	return -1;
}

/* rtc_read
 * DESCRIPTION: block for another RTC interrupt occur
 * INPUTS: none
 * OUTPUTS: 0
 */
int32_t rtc_read(int32_t fd, void* buf, uint32_t nbytes){
    unsigned long flags;
	uint32_t freq, pid, time, i;

    cli_and_save(flags);
	// clear the flag (set interrupt occured to false)
	interrupt_occured = 0;
	restore_flags(flags);

	pid = current_PCB->pid;
	freq = proc_freq[pid];

	if (freq < MIN_FREQ || freq > MAX_FREQ)
	{
		return -1;
	}

	time = phys_freq / freq;
	proc_freq_counter[pid] = 0;
	// wait for another RTC interrupt
	do {
		if (interrupt_occured)
		{
			interrupt_occured = 0;
			for (i = 0; i < NUM_PROC; i++)
			{
				proc_freq_counter[i]++;
			}
		}
	} while (proc_freq_counter[pid] < time);
	return 0;
}

/* rtc_write
 * DESCRIPTION: Change the frequency
 * INPUTS: buf: A pointer
 *		   nbytes: Should be 4 (buf is a uint32_t pointer), or will not do anything
 * OUTPUTS: 0 -- Success
 *			-1 -- Fail
 */
int32_t rtc_write(int32_t fd, const void* buf, uint32_t nbytes){
	uint32_t pid, frequency;
    // check error conditions
	if(buf == NULL || nbytes != sizeof(uint32_t))
		return -1;
	else{
	    //TODO: use userspace address-check after being implemented.
		frequency = *((uint32_t*)buf);			//get frequency
	}

	if (frequency < MIN_FREQ || frequency > MAX_FREQ)
	{
		return -1;
	}

	pid = current_PCB->pid;

	if (pid >= NUM_PROC)
	{
		return -1;
	}
	else
	{
		proc_freq[pid] = frequency;
	}

	if (phys_freq < frequency)
	{
		phys_freq = frequency;
		rtc_changeFreq(phys_freq);
	}

	return nbytes;
}

/* rtc_close
 * DESCRIPTION: does nothing
 * INPUTS: none
 * OUTPUTS: 0
 */
int32_t rtc_close(int32_t fd){
	/* Sanity check */
	if(fd < 0 || fd > 7)
		return -1;

	/* If file is already closed, return -1 */
	if(current_PCB->file_desc_arr[fd].flag == 0)
		return -1;

	/* Clean up file desc array entry */
	current_PCB->file_desc_arr[fd].f_op = NULL;
	current_PCB->file_desc_arr[fd].inode = 0;
	current_PCB->file_desc_arr[fd].f_pos = 0;
	current_PCB->file_desc_arr[fd].flag = 0;
	proc_freq[current_PCB->pid] = 0;

	/* Return 0 on success */
	return 0;
}
