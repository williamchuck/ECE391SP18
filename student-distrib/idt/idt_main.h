/*
 * idt_main.h
 * header files of implementation of idt initialization
 * Authors: Hongxuan Li, Canlin Zhang
 */

#ifndef IDT_MAIN_H
#define IDT_MAIN_H

/*
 * Attention! Current IRQ handlers are defined in interrupt.h
 * and implemented in interrupt.c and interrupt_asm.S
 * Built-in interrupt handlers in ../drivers will not be used. (Unless needed in future.)
 */

/* Include types.h in student-distrib folder */
#include "../types.h"

/* Include standard library functions */
#include "../lib.h"

/* Include file IDT descriptions, data structure and global variable definitions */
#include "../x86_desc.h"

/* Include c implementation and x86 wrappers of idt handlers. */
#include "idt_handler_c.h"
#include "idt_handler_asm.h"

/* Include interrupt handling for keyboard. (Not currently in use.) */
#include "../drivers/ps2_keyboard.h"

#define TASK_GATE_MODE      0x01
#define INTERRUPT_GATE_MODE 0x02
#define TRAP_GATE_MODE      0x03

#define IDT_ENTRY_SIZE        0x100
#define IDT_EXCEPTION_SIZE    0x20
#define IDT_SYSTEM_CALL       0x80
#define IDT_PS2KBD_CALL       0x21
#define IDT_RTC_CALL          0x28

#define INTEL_RESERVED_9      0x9
#define INTEL_RESERVED_15     0xF
#define INTEL_RESERVED_OTHER  0x14

/* Functions for filling the any entry of IDT */
void idt_set_entry(int idt_index, int mode, uint32_t handler_addr, uint16_t seg_selector, uint32_t dpl, uint32_t size, uint32_t present);

/* Function for setting all idt entries */
extern void idt_set_all();

#endif
