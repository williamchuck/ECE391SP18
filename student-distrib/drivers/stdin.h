#ifndef STDIN_H
#define STDIN_H

#include "ps2_keyboard.h"
#include "../lib.h"
#include "../types.h"
#include "stdout.h"

#define TEXT_IN_ADDR       0x03D4
#define CURSOR_HIGH        0x0E
#define CURSOR_LOW         0x0F
#define VGA_WIDTH          80
#define VGA_HEIGHT         25
#define ASCII_NL		       0x0A
#define TERM_EOF           0xFF
#define TERM_IN_FD         0x00
#define TERM_OUT_FD        0x01

extern int32_t stdin_open(const uint8_t* filename);

extern int32_t stdin_close(int32_t fd);

extern int32_t stdin_read(int32_t fd, void* buf, int32_t nbytes);

extern int32_t stdin_write(int32_t fd, const void* buf, int32_t nbytes);

#endif