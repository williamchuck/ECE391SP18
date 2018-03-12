/*
 * rtc.h
 *  RTC device driver stuff
 *  Created on: Mar 11, 2018
 *  Author: Hongxuan Li.
 */

#ifndef RTC_H
#define RTC_H

/*
 * For checkpoint 2
int rtc_open();
int rtc_close();
int rtc_read();
int rtc_write();
*/
/* To enable periodic rtc interrupt */
int rtc_enable_interrupt();


#endif /* RTC_H */
