#ifndef PS2_KEYBOARD_H
#define PS2_KEYBOARD_H

#include "../i8259.h"
#include "../lib.h"
#include "../types.h"

/*
 * This file is for keyboard initialization and interrupt handling.
 */

#define KBD_IRQ     0x01

void ps2_keyboard_init();

unsigned char ps2_keyboard_getscancode();

unsigned char ps2_keyboard_getchar(unsigned char scancode);

extern void int_ps2kbd_c();

#endif
