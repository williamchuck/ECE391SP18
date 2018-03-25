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

/* rtc_open
 * DESCRIPTION: Set the RTC frequency to default value 2
 * INPUTS: none
 * OUTPUTS: 0
 */
int32_t rtc_open(const int8_t* fname);

/* rtc_read
 * DESCRIPTION: wait for another RTC interrupt occur
 * INPUTS: none
 * OUTPUTS: 0
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);

/* rtc_write
 * DESCRIPTION: Change the frequency
 * INPUTS: buf: A pointer
 *		   nbyte: Should be 4 (NBYTE_DEFAULT_VAL), or will not do anything
 * OUTPUTS: 0 -- Success;
  			-1 -- Fail
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);

/* rtc_close
 * DESCRIPTION: Set the RTC frequency back to default value 2
 * INPUTS: none
 * OUTPUTS: 0
 */
int32_t rtc_close(int32_t fd);


#endif /* RTC_H */
