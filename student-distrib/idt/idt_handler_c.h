#ifndef IDT_HANDLER_C_H
#define IDT_HANDLER_C_H

/*
 * idt/idt_handler_c.h
 * Release version - Caffine_Theanine beta 0.0.0.1
 * Latest revision by Canlin Zhang (canlinz2).
 * Please maintain a good working habit: Write comments! Update versions! (if applicable, don't use magic numbers.)
 */

/*
 * This file contains the declaration of c functions of interrupt (or exception) handlers.
 * More entries can be added in these steps:
 * add a .global variable in format int_(num)_asm in idt_handler_asm.S.
 * add a function definition of the interrupt handler (x86) in idt_handler_asm.h
 * add a function definition of the interrupt handler (c) in idt_handler_c.h
 * add implementation of the interrupt handler (x86) in idt_handler_asm.S
 * add implementation of the interrupt handler (c) in idt_handler_c.h
 * add the interrupt entry to idt_table. (by changing idt_set_all in idt_main.c)
 */


/* Include useful functions from c library, however, not entirely necessary. */
#include "../lib.h"
/* Include types in stdint.h. Again, not entirely necessary. */
#include "../types.h"

/* Declarations of interrupt handlers. */
extern void int_0_c();
extern void int_1_c();
extern void int_2_c();
extern void int_3_c();
extern void int_4_c();
extern void int_5_c();
extern void int_6_c();
extern void int_7_c();
extern void int_8_c();
extern void int_9_c();
extern void int_A_c();
extern void int_B_c();
extern void int_C_c();
extern void int_D_c();
extern void int_E_c();
extern void int_F_c();
extern void int_10_c();
extern void int_11_c();
extern void int_12_c();
extern void int_13_c();
extern void int_14_c();
extern void int_15_c();
extern void int_16_c();
extern void int_17_c();
extern void int_18_c();
extern void int_19_c();
extern void int_1A_c();
extern void int_1B_c();
extern void int_1C_c();
extern void int_1D_c();
extern void int_1E_c();
extern void int_1F_c();

#endif
