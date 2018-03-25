/*
 * rtc.h
 *  RTC device driver stuff
 *  Created on: Mar 11, 2018
 *  Author: Hongxuan Li, Linz Chiang
 */

#ifndef RTC_H
#define RTC_H


#include "../lib.h"
#include "../idt/interrupt.h"
#include "../types.h"

/* To enable periodic rtc interrupt */
int rtc_enable_interrupt();

/*Definitions for RTC*/
#define RTC_REG_A			0x8A
#define	RTC_REG_B 			0x8B
#define RTC_REG_C 			0x0C
#define RTC_PORT 			0x70
#define COMS_PORT 			0x71
#define RTC_EOI				8
#define DEFAULT_FREQ 		2
#define NBYTE_DEFAULT_VAL 	4

// Active Low Implementation Constant
#define ACT_LOW				0xFF
//The low 4 bits are between 2 and 15
#define RTC_FRQ_ACT_LOW 	0xF0

// Constants for frequency calculation
#define MAX_FREQ			1024
#define FREQ_RATE_CALC		32768


/* open_RTC
 * DESCRIPTION: Set the RTC frequency to default value 2
 * INPUTS: none
 * OUTPUTS: 0
 */
int32_t open_RTC();

/* changeFreq_RTC
 * DESCRIPTION: Change the RTC frequency
 * INPUTS: freq -- the frequency to be set
 * OUTPUTS: None
 */
void changeFreq_RTC(uint32_t freq);

/* read_RTC
 * DESCRIPTION: wait for another RTC interrupt occur
 * INPUTS: none
 * OUTPUTS: 0
 */
int32_t read_RTC();

/* write_RTC
 * DESCRIPTION: Change the frequency
 * INPUTS: buf: A pointer
 *		   nbyte: Should be 4 (NBYTE_DEFAULT_VAL), or will not do anything
 * OUTPUTS: 0 -- Success;
  			-1 -- Fail
 */
int32_t write_RTC(const void* buf, int32_t nbytes);

/* close_RTC
 * DESCRIPTION: Set the RTC frequency back to default value 2
 * INPUTS: none
 * OUTPUTS: 0
 */
int32_t close_RTC();


#endif /* RTC_H */
