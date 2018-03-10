#ifndef IDT_MAIN_H
#define IDT_MAIN_H

/* Include types.h in student-distrib folder */
#include "../types.h"

/* Include standard library functions */
#include "../lib.h"

/* Include file IDT descriptions, data structure and global variable definitions */
#include "../x86_desc.h"

#include "idt_handler_c.h"

#include "idt_handler_asm.h"

#define TASK_GATE_MODE      0x01
#define INTERRUPT_GATE_MODE 0x02
#define TRAP_GATE_MODE      0x03

#define IDT_ENTRY_SIZE        0x100
#define IDT_EXCEPTION_SIZE    0x20
#define IDT_SYSTEM_CALL       0x80

#define INTEL_RESERVED_9      0x9
#define INTEL_RESERVED_15     0xF
#define INTEL_RESERVED_OTHER  0x14

/* Functions for filling the any entry of IDT */
void idt_set_entry(int idt_index, int mode, uint32_t handler_addr, uint16_t seg_selector, uint32_t dpl, uint32_t size, uint32_t present);

/* Function for setting all idt entries */
extern void idt_set_all();

#endif
