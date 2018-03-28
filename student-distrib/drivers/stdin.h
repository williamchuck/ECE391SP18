/*
 * stdin.h
 * Header file of standard input (terminal driver for this checkpoint)
 * Author: Canlin Zhang
 */
#ifndef STDIN_H
#define STDIN_H

 /* Include keyboard driver, standard library, types and standard input. */
#include "ps2_keyboard.h"
#include "../lib.h"
#include "../types.h"
#include "stdout.h"

/* Some constants */
/* VGA data ports and index for cursor control */
#define TEXT_IN_ADDR       0x03D4
#define TEXT_IN_DATA	   0x03D5
#define CURSOR_HIGH        0x0E
#define CURSOR_LOW         0x0F

/* Number of columns and rows in VGA text mode */
#define VGA_WIDTH          80
#define VGA_HEIGHT         25

/* ASCII constants, We use 0xFF as EOF in this system */
#define ASCII_NL		   0x0A
#define TERM_EOF           0xFF

/* File Descriptors for Input and Output */
#define TERM_IN_FD         0x00
#define TERM_OUT_FD        0x01

/* Buffer size is 128 bytes */
#define BUF_SIZE		   128

/* Keycode of enter key */
#define KBDENP			   0x1C

/* Open handler for standard input */
extern int32_t stdin_open(const uint8_t* filename);

/* Close handler for standard input */
extern int32_t stdin_close(int32_t fd);

/* Read handler for standard input */
extern int32_t stdin_read(int32_t fd, void* buf, int32_t nbytes);

/* Write handler for standard input */
extern int32_t stdin_write(int32_t fd, const void* buf, int32_t nbytes);

#endif
