#ifndef IDT_HANDLER_ASM_H
#define IDT_HANDLER_ASM_H

/*
 * idt/idt_handler_asm.h
 * Release version - Caffine_Theanine beta 0.0.0.1
 * Latest revision by Canlin Zhang (canlinz2).
 * Please maintain a good working habit: Write comments! Update versions! (if applicable, don't use magic numbers.)
 */

/*
 * This file contains the declaration of x86 linkage functions of interrupt (or exception) handlers.
 * More entries can be added in these steps:
 * add a .global variable in format int_(num)_asm in idt_handler_asm.S.
 * add a function definition of the interrupt handler (x86) in idt_handler_asm.h
 * add a function definition of the interrupt handler (c) in idt_handler_c.h
 * add implementation of the interrupt handler (x86) in idt_handler_asm.S
 * add implementation of the interrupt handler (c) in idt_handler_c.h
 * add the interrupt entry to idt_table. (by changing idt_set_all in idt_main.c)
 */

/* Declarations of interrupt assembly linage functions. */
extern void int_0_asm();
extern void int_1_asm();
extern void int_2_asm();
extern void int_3_asm();
extern void int_4_asm();
extern void int_5_asm();
extern void int_6_asm();
extern void int_7_asm();
extern void int_8_asm();
extern void int_9_asm();
extern void int_A_asm();
extern void int_B_asm();
extern void int_C_asm();
extern void int_D_asm();
extern void int_E_asm();
extern void int_F_asm();
extern void int_10_asm();
extern void int_11_asm();
extern void int_12_asm();
extern void int_13_asm();
extern void int_14_asm();
extern void int_15_asm();
extern void int_16_asm();
extern void int_17_asm();
extern void int_18_asm();
extern void int_19_asm();
extern void int_1A_asm();
extern void int_1B_asm();
extern void int_1C_asm();
extern void int_1D_asm();
extern void int_1E_asm();
extern void int_1F_asm();

#endif
