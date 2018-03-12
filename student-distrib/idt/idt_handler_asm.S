/*
 * idt/idt_handler_asm.S
 * x86 linkage for exception handler functions.
 * Author: Canlin Zhang, Hongxuan Li.
 */

/*
 * This file contains the implemantation of assembly linkage for interrupt (or exception) handlers.
 * More entries can be added in these steps:
 * add a .global variable in format int_(num)_asm in this file.
 * add a function definition of the interrupt handler (x86) in idt_handler_asm.h
 * add a function definition of the interrupt handler (c) in idt_handler_c.h
 * add implementation of the interrupt handler (x86) in idt_handler_asm.c
 * add implementation of the interrupt handler (c) in idt_handler_c.h
 * add the interrupt entry to idt_table.
 */

/*
 * Handler address of each interrupt/exception.
 * Made global to be used by idt_main.c to initialize idt entry.
 */
.globl int_0_asm
.globl int_1_asm
.globl int_2_asm
.globl int_3_asm
.globl int_4_asm
.globl int_5_asm
.globl int_6_asm
.globl int_7_asm
.globl int_8_asm
.globl int_9_asm
.globl int_A_asm
.globl int_B_asm
.globl int_C_asm
.globl int_D_asm
.globl int_E_asm
.globl int_F_asm
.globl int_10_asm
.globl int_11_asm
.globl int_12_asm
.globl int_13_asm
.globl int_14_asm
.globl int_15_asm
.globl int_16_asm
.globl int_17_asm
.globl int_18_asm
.globl int_19_asm
.globl int_1A_asm
.globl int_1B_asm
.globl int_1C_asm
.globl int_1D_asm
.globl int_1F_asm

/* (x86 wrapper for) Handler for system call */
.globl int_80_asm

/*
 * irq handler stubs.
 * Made global to be used by idt_main.c to initialize idt entry.
 */
.globl int_20_asm
.globl int_21_asm
.globl int_22_asm
.globl int_23_asm
.globl int_24_asm
.globl int_25_asm
.globl int_26_asm
.globl int_27_asm
.globl int_28_asm
.globl int_29_asm
.globl int_2A_asm
.globl int_2B_asm
.globl int_2C_asm
.globl int_2D_asm
.globl int_2E_asm
.globl int_2F_asm

.align 4


/*
 * Implementation of interrupts' assembly linkage:
 * Just callee save and call correspoding c functions.
 */

/* Interrupt 0 - Divide Error Exception (#DE) */
int_0_asm:
  pushal
  call int_0_c
  popal
  iret

/* Interrupt 1 - Debug Exception (#DB)*/
int_1_asm:
  pushal
  call int_1_c
  popal
  iret

/* Interrupt 2 - NMI Interrupt */
int_2_asm:
  pushal
  call int_2_c
  popal
  iret

/* Interrupt 3 - Breakpoint Exception (#BP) */
int_3_asm:
  pushal
  call int_3_c
  popal
  iret

/* Interrupt 4 - Overflow Exception (#OF) */
int_4_asm:
  pushal
  call int_4_c
  popal
  iret

/* Interrupt 5 - BOUND Range Exceeded Exception (#BR) */
int_5_asm:
  pushal
  call int_5_c
  popal
  iret

/* Interrupt 6 - Invalid Opcode Exception (#UD) */
int_6_asm:
  pushal
  call int_6_c
  popal
  iret

/* Interrupt 7 - Device Not Available Exception (#NM) */
int_7_asm:
  pushal
  call int_7_c
  popal
  iret

/* Interrupt 8 - Double Fault Exception (#DF) */
int_8_asm:
  pushal
  call int_8_c
  popal
  iret

/* Interrupt 9 - Coprocessor Segment Overrun */
/* This Interrupt is reserved by Shintel */
int_9_asm:
  pushal
  call int_9_c
  popal
  iret

/* Interrupt 10 - Invalid TSS Exception (#TS) */
int_A_asm:
  pushal
  call int_A_c
  popal
  iret

/* Interrupt 11 - Segment Not Present (#NP) */
int_B_asm:
  pushal
  call int_B_c
  popal
  iret

/* Interrupt 12 - Stack Fault Exception (#SS) */
int_C_asm:
  pushal
  call int_C_c
  popal
  iret

/* Interrupt 13 - General Protection Exception (#GP) */
int_D_asm:
  pushal
  call int_D_c
  popal
  iret

/* Interrupt 14 - Page Fault Exception (#PF) */
int_E_asm:
  pushal
  call int_E_c
  popal
  iret

/* Interrupt 15 - Reserved by Shintel */
int_F_asm:
  pushal
  call int_F_c
  popal
  iret

/* Interrupt 16 - x87 FPU Floating-Point Error (#MF) */
int_10_asm:
  pushal
  call int_10_c
  popal
  iret

/* Interrupt 17 - Alignment Check Exception (#AC) */
int_11_asm:
  pushal
  call int_11_c
  popal
  iret

/* Interrupt 18 - Machine check exception (#MC) */
int_12_asm:
  pushal
  call int_12_c
  popal
  iret

/* Interrupt 19 - SIMD Floating Point Exception (#XF) */
int_13_asm:
  pushal
  call int_13_c
  popal
  iret

/* Interrupt 20 - Reserved by Shintel */
int_14_asm:
  pushal
  call int_14_c
  popal
  iret

/* Interrupt 21 - Reserved by Shintel */
int_15_asm:
  pushal
  call int_15_c
  popal
  iret

/* Interrupt 22 - Reserved by Shintel */
int_16_asm:
  pushal
  call int_16_c
  popal
  iret

/* Interrupt 23 - Reserved by Shintel */
int_17_asm:
  pushal
  call int_17_c
  popal
  iret

/* Interrupt 24 - Reserved by Shintel */
int_18_asm:
  pushal
  call int_18_c
  popal
  iret

/* Interrupt 25 - Reserved by Shintel */
int_19_asm:
  pushal
  call int_19_c
  popal
  iret

/* Interrupt 26 - Reserved by Shintel */
int_1A_asm:
  pushal
  call int_1A_c
  popal
  iret

/* Interrupt 27 - Reserved by Shintel */
int_1B_asm:
  pushal
  call int_1B_c
  popal
  iret

/* Interrupt 28 - Reserved by Shintel */
int_1C_asm:
  pushal
  call int_1C_c
  popal
  iret

/* Interrupt 29 - Reserved by Shintel */
int_1D_asm:
  pushal
  call int_1D_c
  popal
  iret

/* Interrupt 30 - Reserved by Shintel */
int_1E_asm:
  pushal
  call int_1E_c
  popal
  iret

/* Interrupt 31 - Reserved by Shintel */
int_1F_asm:
  pushal
  call int_1F_c
  popal
  iret

/* Interrupt 128 - System Call */
int_80_asm:
  pushal
  call int_80_c
  popal
  iret

/* irq handler stub */
/*
 * Each value pushed onto stack corresponds to a value
 * in irq handler address array, defined in idt_main.c
 * For MP3.1, only interrupt 0x21, 0x22 and 0x28 are used.
 */
int_20_asm:
	pushl $0
	jmp common_interrupt

/* Keyboard Interrupt */
int_21_asm:
	pushl $1
	jmp common_interrupt

/* Slave PIC Interrupt */
int_22_asm:
	pushl $2
	jmp common_interrupt

int_23_asm:
	pushl $3
	jmp common_interrupt

int_24_asm:
	pushl $4
	jmp common_interrupt

int_25_asm:
	pushl $5
	jmp common_interrupt

int_26_asm:
	pushl $6
	jmp common_interrupt

int_27_asm:
	pushl $7
	jmp common_interrupt

/* RTC Interrupt */
int_28_asm:
	pushl $8
	jmp common_interrupt

int_29_asm:
	pushl $9
	jmp common_interrupt

int_2A_asm:
	pushl $10
	jmp common_interrupt

int_2B_asm:
	pushl $11
	jmp common_interrupt

int_2C_asm:
	pushl $12
	jmp common_interrupt

int_2D_asm:
	pushl $13
	jmp common_interrupt

int_2E_asm:
	pushl $14
	jmp common_interrupt

int_2F_asm:
	pushl $15
	jmp common_interrupt
