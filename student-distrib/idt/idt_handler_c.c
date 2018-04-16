
/* Include useful functions from c library, however, not entirely necessary. */
#include "../lib.h"
/* Include types in stdint.h. Again, not entirely necessary. */
#include "../types.h"
/*
 * idt/idt_handler_c.c
 * C implementation of system exception handlers.
 * Latest revision by Canlin Zhang, Hongxuan Li.
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

 #include "../i8259.h"
#include "../process/process.h"
#include "../syscall/syscall.h"

void int_0_c()
{
  printf("Interrupt 0 - Fault - Divide Error Exception (#DE).\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_1_c()
{
  printf("Interrupt 1 - Fault/Trap - Debug Exception (#DB).\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_2_c()
{
  printf("Interrupt 2 - N/A - NMI Interrupt.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_3_c()
{
  printf("Interrupt 3 - Trap - Breakpoint Exception.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_4_c()
{
  printf("Interrupt 4 - Trap - Overflow Exception.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_5_c()
{
  printf("Interrupt 5 - Fault - BOUND Range Exceeded Exception.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_6_c()
{
  printf("Interrupt 6 - Fault - Invalid Opcode Exception.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_7_c()
{
  printf("Interrupt 7 - Fault - Device Not Available Exception.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_8_c()
{
  printf("Interrupt 8 - Abort - Double Fault Exception.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_9_c()
{
  printf("Interrupt 9 - Abort - Coprocessor Segment Overrun.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_A_c()
{
  printf("Interrupt 10 - Fault - Invalid TSS Exception.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_B_c()
{
  printf("Interrupt 11 - Fault - Segment Not Present.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_C_c()
{
  printf("Interrupt 12 - Fault - Stack Fault Exception.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_D_c()
{
  printf("Interrupt 13 - Fault - General Protection Exception.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_E_c()
{
  printf("Interrupt 14 - Fault - Page-Fault Exception.\n");
  //halt application for segfault
  system_internal_halt(256);
  asm volatile ("1: hlt; jmp 1b;");
}
void int_F_c()
{
  printf("Interrupt 15 - Reserved by Shintel.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_10_c()
{
  printf("Interrupt 16 - Fault - x87 FPU Floating-Point Error.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_11_c()
{
  printf("Interrupt 17 - Fault - Alignment Check Exception.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_12_c()
{
  printf("Interrupt 18 - Abort - Machine-Check Exception.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_13_c()
{
  printf("Interrupt 19 - Fault - SIMD Floating Point Exception.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_14_c()
{
  printf("Interrupt 20 - Reserved by Shintel.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_15_c()
{
  printf("Interrupt 21 - Reserved by Shintel.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_16_c()
{
  printf("Interrupt 22 - Reserved by Shintel.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_17_c()
{
  printf("Interrupt 23 - Reserved by Shintel.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_18_c()
{
  printf("Interrupt 24 - Reserved by Shintel.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_19_c()
{
  printf("Interrupt 25 - Reserved by Shintel.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_1A_c()
{
  printf("Interrupt 26 - Reserved by Shintel.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_1B_c()
{
  printf("Interrupt 27 - Reserved by Shintel.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_1C_c()
{
  printf("Interrupt 28 - Reserved by Shintel.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_1D_c()
{
  printf("Interrupt 29 - Reserved by Shintel.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_1E_c()
{
  printf("Interrupt 30 - Reserved by Shintel.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
void int_1F_c()
{
  printf("Interrupt 31 - Reserved by Shintel.\n");
  asm volatile ("1: hlt; jmp 1b;");
}
