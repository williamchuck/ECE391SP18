/*
 * ps2_keyboard.h
 * Definitions of keyboard initialization and interrupt handling.
 * Author: Canlin Zhang
 */
#ifndef PS2_KEYBOARD_H
#define PS2_KEYBOARD_H

/* Include PIC, library, stdint and interrupt. */
#include "../i8259.h"
#include "../lib.h"
#include "../types.h"
#include "../idt/interrupt.h"

/* Keyboard IRQ number = 1 */
#define KBD_IRQ     0x01

/* Initialize keyboard */
void ps2_keyboard_init();

/* Get Raw scan code from keyboard */
unsigned char ps2_keyboard_getscancode();

/* Get converted ASCII code, given scan code. */
unsigned char ps2_keyboard_getchar(unsigned char scancode);

/* Interrupt handler for keyboard */
extern void int_ps2kbd_c();

#endif
