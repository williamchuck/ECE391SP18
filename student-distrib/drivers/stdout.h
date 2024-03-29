/*
 * stdout.h
 * Header file for standard output (terminal driver for this checkpoint)
 * Author: Canlin Zhang
 */
#ifndef STDOUT_H
#define STDOUT_H

 /* Include keyboard driver, standard library and standard types */
#include "ps2_keyboard.h"
#include "../lib.h"
#include "../types.h"

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
#define ASCII_SPACE        0x20

/* File Descriptors for Input and Output */
#define TERM_IN_FD         0x00
#define TERM_OUT_FD        0x01

/* Buffer size is 128 bytes */
#define BUF_SIZE		   128
#define VMEM_SIZE		   4000

/* There are 3 terminals in the system. */
#define TERM_NUM		   3

/* Physical address of video memory in text mode. */
#define VIDEO			   0xB8000

/* Terminal buffer and index, 2D array for 3 terminals. */
unsigned char term_buf[TERM_NUM][BUF_SIZE];
int term_buf_index[TERM_NUM];

/* Current terminal. */
extern int cur_term;

/* Open sys. call handler for standard output */
extern int32_t stdout_open(const int8_t* filename);

/* Close sys. call handler for standard output */
extern int32_t stdout_close(int32_t fd);

/* Read sys. call handler for standard output */
extern int32_t stdout_read(int32_t fd, void* buf, uint32_t nbytes);

/* Write sys. call handler for standard output */
extern int32_t stdout_write(int32_t fd, const void* buf, uint32_t nbytes);

/* Helper function. Resets cursor to (0, 0) */
void cursor_reset(int term);

/*
 * Helper function. Updates current VGA cursor position
 * according to actual cursor position. (In lib.c)
 */
void cursor_update(int term);

/* Helper function. Initializes Terminal by clearing screen and resetting cursor. */
void term_init(int term);

/* Helper function. Moves cursor and (if applicable) delete char on screen and in buffer */
void term_del(int term);

/* Helper function. Switch to designated terminal. */
void term_switch(int term);

#endif
