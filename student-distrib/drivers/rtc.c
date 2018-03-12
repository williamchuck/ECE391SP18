/*
 * rtc.c
 *
 *  Created on: Mar 11, 2018
 *      Author: Dennis
 */


#include "rtc.h"
#include "../lib.h"
#include "../idt/interrupt.h"
extern void test_interrupts(void);
void rtc_isr(){
    printf("rtc int");
}
int rtc_enable_interrupt(){
    //referenced from https://wiki.osdev.org/RTC
    unsigned long flags;
    cli_and_save(flags);

    outb(0x70, 0x8B);
    char regB=inb(0x71);
    outb(0x70, 0x8B);
    outb(0x71, regB | 0x40);

    restore_flags(flags);
    return request_irq(8, &rtc_isr);
}
