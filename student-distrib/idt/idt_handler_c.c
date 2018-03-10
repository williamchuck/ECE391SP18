#include "idt_handler_c.h"

/*
 * idt/idt_handler_c.c
 * Release version - Caffine_Theanine beta 0.0.0.1
 * Latest revision by Canlin Zhang (canlinz2).
 * Please maintain a good working habit: Write comments! Update versions! (if applicable, don't use magic numbers.)
 */

/*
 * This file contains the implementation of c functions of interrupt (or exception) handlers.
 * More entries can be added in these steps:
 * add a .global variable in format int_(num)_asm in idt_handler_asm.S.
 * add a function definition of the interrupt handler (x86) in idt_handler_asm.h
 * add a function definition of the interrupt handler (c) in idt_handler_c.h
 * add implementation of the interrupt handler (x86) in idt_handler_asm.S
 * add implementation of the interrupt handler (c) in idt_handler_c.h
 * add the interrupt entry to idt_table. (by changing idt_set_all in idt_main.c)
 */

void int_0_c()
{
  printf("Interrupt 0 - Fault - Divide Error Exception (#DE).\n");
}
void int_1_c()
{
  printf("Interrupt 1 - Fault/Trap - Debug Exception (#DB).\n");
}
void int_2_c()
{
  printf("Interrupt 2 - N/A - NMI Interrupt.\n");
}
void int_3_c()
{
  printf("Interrupt 3 - Trap - Breakpoint Exception.\n");
}
void int_4_c()
{
  printf("Interrupt 4 - Trap - Overflow Exception.\n");
}
void int_5_c()
{
  printf("Interrupt 5 - Fault - BOUND Range Exceeded Exception.\n");
}
void int_6_c()
{
  printf("Interrupt 6 - Fault - Invalid Opcode Exception.\n");
}
void int_7_c()
{
  printf("Interrupt 7 - Fault - Device Not Available Exception.\n");
}
void int_8_c()
{
  printf("Interrupt 8 - Abort - Double Fault Exception.\n");
}
void int_9_c()
{
  printf("Interrupt 9 - Abort - Coprocessor Segment Overrun.\n");
}
void int_A_c()
{
  printf("Interrupt 10 - Fault - Invalid TSS Exception.\n");
}
void int_B_c()
{
  printf("Interrupt 11 - Fault - Segment Not Present.\n");
}
void int_C_c()
{
  printf("Interrupt 12 - Fault - Stack Fault Exception.\n");
}
void int_D_c()
{
  printf("Interrupt 13 - Fault - General Protection Exception.\n");
}
void int_E_c()
{
  printf("Interrupt 14 - Fault - Page-Fault Exception.\n");
}
void int_F_c()
{
  printf("Interrupt 15 - Reserved by Shintel.\n");
}
void int_10_c()
{
  printf("Interrupt 16 - Fault - x87 FPU Floating-Point Error.\n");
}
void int_11_c()
{
  printf("Interrupt 17 - Fault - Alignment Check Exception.\n");
}
void int_12_c()
{
  printf("Interrupt 18 - Abort - Machine-Check Exception.\n");
}
void int_13_c()
{
  printf("Interrupt 19 - Fault - SIMD Floating Point Exception.\n");
}
void int_14_c()
{
  printf("Interrupt 20 - Reserved by Shintel.\n");
}
void int_15_c()
{
  printf("Interrupt 21 - Reserved by Shintel.\n");
}
void int_16_c()
{
  printf("Interrupt 22 - Reserved by Shintel.\n");
}
void int_17_c()
{
  printf("Interrupt 23 - Reserved by Shintel.\n");
}
void int_18_c()
{
  printf("Interrupt 24 - Reserved by Shintel.\n");
}
void int_19_c()
{
  printf("Interrupt 25 - Reserved by Shintel.\n");
}
void int_1A_c()
{
  printf("Interrupt 26 - Reserved by Shintel.\n");
}
void int_1B_c()
{
  printf("Interrupt 27 - Reserved by Shintel.\n");
}
void int_1C_c()
{
  printf("Interrupt 28 - Reserved by Shintel.\n");
}
void int_1D_c()
{
  printf("Interrupt 29 - Reserved by Shintel.\n");
}
void int_1E_c()
{
  printf("Interrupt 30 - Reserved by Shintel.\n");
}
void int_1F_c()
{
  printf("Interrupt 31 - Reserved by Shintel.\n");
}
