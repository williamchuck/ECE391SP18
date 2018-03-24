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
#define RTC_DEFAULT_FREQ 	2
#define NBYTE_DEFAULT_VAL 	4

/*Active Low Implementation Constant*/
#define ACT_LOW				0xFF
#define RTC_FRQ_ACT_LOW 	0xF0 //The low 4 bits are important for frequency as it is between 2 and 15

/*Constants for frequency calculation*/
#define MAX_FREQ			1024
#define FREQ_FOR_RATE_CALC	32768

/* initialize_RTC
 *
 * Description: Initialize the RTC
 * Inputs: None
 * Outputs: None
 * Side Effects: As description
 */
void initialize_RTC();

int32_t open_RTC(const uint8_t* filename);

void changeFreq_RTC(uint32_t freq);

int32_t read_RTC();

int32_t write_RTC(int32_t fd, const void* buf, int32_t nbytes);

int32_t close_RTC();

void RTC_handler();

#endif /* RTC_H */
