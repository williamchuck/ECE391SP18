/*
 * stdout.c
 * Implementation of standard output (Terminal driver for this checkpoint)
 * Author: Canlin Zhang
 */
#include "stdout.h"
#include "../fs/fs.h"
#include "../page/page.h"
#include "../lib.h"
#include "../process/process.h"

/* Standard output file operation structure. */
static file_op_t stdout_file_op = {
	.open = stdout_open,
	.read = stdout_read,
	.write = stdout_write,
	.close = stdout_close
};

/* Pointer to file operation structure. */
file_op_t* stdout_op = &stdout_file_op;

int cur_term;

 /*
  * stdout_open
  * DESCRIPTION: "open" system call handler for standard output.
  * INPUT: filename - file to be opened.
  * OUTPUT: none.
  * RETURN VALUE: 0 on success. -1 on failure.
  * SIDE EFFECTS: Return value affects system call.
  * NOTES: More functionality to be implemented. This function now does nothing and returns 0.
  */
int32_t stdout_open(const int8_t* filename)
{
	return 0;
}

/*
 * stdout_close
 * DESCRIPTION: "close" system call handler for standard output.
 * INPUT: fd - file descriptor.
 * OUTPUT: none.
 * RETURN VALUE: 0 on success. -1 on failure.
 * SIDE EFFECTS: Return value affects system call.
 * NOTES: More functionality to be implemented. This function now does nothing and returns 0.
 */
int32_t stdout_close(int32_t fd)
{
	return 0;
}

/*
 * stdout_read
 * DESCRIPTION: "read" system call handler for standard output
 * INPUT: fd - file descriptor
 *		  buf - incoming data stream (buffer) to be read
 *		  nbytes - number of bytes to be read
 * OUTPUT: none.
 * RETURN VALUE: number of bytes read on success, -1 on failure.
 * SIDE EFFECTS: Return value affects system call.
 * NOTES: The system is NOT supposed to call read for standard input.
 *		  This function will only return -1.
 */
int32_t stdout_read(int32_t fd, void* buf, uint32_t nbytes)
{
	return -1;
}

/*
 * stdout_write
 * DESCRIPTION: "write" system call handler for standard output
 * INPUT: fd - file descriptor
 *		  buf - data stream to be output (displayed onto terminal)
 *		  nbytes - number of bytes to be displayed onto terminal
 * OUTPUT: Data in buf will be displayed on terminal upon success.
 * RETURN VALUE: number of bytes displayed on success. -1 on failure.
 * SIDE EFFECTS: Data will be displayed onto terminal.
 */
int32_t stdout_write(int32_t fd, const void* buf, uint32_t nbytes)
{
	/* Loop var. */
	int i;
	/* Pointer to buffer (for type casting) */
	unsigned char* buf_ptr;

	/* Cast pointer to output stream from const void* to unsigned char* */
	buf_ptr = (unsigned char*)buf;

	/* If pointer to buffer is NULL, return error. */
	if (buf_ptr == NULL)
		return -1;

	/* Loop through bytes to display the data onto terminal */
	/*
	 * Note: We assume nbytes is smaller than or equal to actual buffer size.
	 * Also, by detecting EOF, we can avoid most of potential undefined behavior.
	 * (Better options WIP...)
	 */
	for (i = 0; i < nbytes; i++)
	{
		/* If we have reached the end of file, return number of bytes displayed */
		if (buf_ptr[i] == TERM_EOF)
			return i;

		/* Display data onto terminal */
		putc(buf_ptr[i]);
	}
	/* If we displayed all the data successfully, return nbytes */
	return nbytes;
}

/*
 * cursor_reset
 * DESCRIPTION: Resets terminal cursor to position (0, 0)
 * INPUT: none.
 * OUTPUT: none.
 * RETURN VALUE: none.
 * SIDE EFFECTS: Resets cursor of the terminal to position (0, 0)
 */
void cursor_reset(int term)
{
	/* Vars. for x, y coords. and overall position. */
	int x;
	int y;
	uint16_t pos;

	/* Set coordinates to (0, 0) */
	x = 0;
	y = 0;

	/* Calculate new positions of VGA cursor */
	pos = y * VGA_WIDTH + x;

	/* Set "Real" cursor position for putc (in lib.c) */
	set_xy(x, y, term);

	/* Write new cursor position to Cursor location low and high registers */
	outb(CURSOR_LOW, TEXT_IN_ADDR);
	outb((uint8_t)(pos & TERM_EOF), TEXT_IN_DATA);
	outb(CURSOR_HIGH, TEXT_IN_ADDR);
	outb((uint8_t)((pos >> 8) & TERM_EOF), TEXT_IN_DATA);
}

/*
 * term_init
 * DESCRIPTION: Initialize terminal display
 * INPUT: none.
 * OUTPUT: none.
 * RETURN VALUE: none.
 * SIDE EFFECTS: Initialize terminal by clearing screen, resetting cursor and clearning buffer.
 */
void term_init(int term)
{
	/* Loop var */
	int i;

	/* Reset cursor */
	cursor_reset(term);

	/* Map memory - May not be necessary. */
	set_4KB(video_term[term], video_term[term], 0);

	/* Clear termimal pages */
	clear_term(term);

	/* Clear video memory */
	clear();

	/* 
	 * Clearing buffer by initializing buffer to EOF. 
	 * (Not to NULL because NULL can be displayed) 
	 */
	for (i = 0; i < BUF_SIZE; i++)
		term_buf[term][i] = TERM_EOF;

	/* Reset buffer index to 0 */
	term_buf_index[term] = 0;
}

/*
 * term_del
 * DESCRIPTION: Handler for deleting one char on screen and buffer when backspace is pressed.
 * INPUT: none.
 * OUTPUT: none.
 * RETURN VALUE: none.
 * SIDE EFFECTS: Erases a char from screen (and buffer).
 */
void term_del(int term)
{
	/* If keyboard buffer is empty, do nothing. */
	if (term_buf_index[term] == 0)
		return;

	/* Coordinates and location var. */
	int x;
	int y;
	uint16_t pos;

	/* Get current cursor coordinates and calculate position */
	x = get_x(term);
	y = get_y(term);
	pos = y * VGA_WIDTH + x;

	/* Cursor manipulation is separated from buffer manipulation. */
	/* If current position is 0, do nothing to cursor. */
	if (pos != 0)
	{
		/* Decrement position and calculate new cursor coordinates. */
		pos--;
		x = pos % VGA_WIDTH;
		y = pos / VGA_WIDTH;

		/* Set new cursor position to write over (delete) char on screen. */
		set_xy(x, y, term);

		/* Use NULL character to erase character to be deleted */
		kbd_putc(0x00);

		/* Set coordinates to decremented position again. */
		set_xy(x, y, term);
	}

	/* Delete current char in buffer and decrement count and index. */
	term_buf[term][term_buf_index[term] - 1] = TERM_EOF;
	term_buf_index[term]--;
	return;
}

/*
 * cursor_update
 * DESCRIPTION: updates cursor position so that VGA cursor position matches real cursor position.
 * INPUT: none.
 * OUTPUT: none.
 * RETURN VALUE: none.
 * SIDE EFFECTS: updates cursor on screen. 
 */
void cursor_update(int term)
{
	/* Coordinates and position of cursors. */
	int x;
	int y;
	uint16_t pos;

	if(term != cur_term)
		return;

	/* Get real cursor positions and calculate corresponding cursor position. */
	x = get_x(term);
	y = get_y(term);
	pos = y * VGA_WIDTH + x;

	/* Write new cursor position to Cursor location low and high registers */
	if (term == cur_term)
	{
		outb(CURSOR_LOW, TEXT_IN_ADDR);
		outb((uint8_t)(pos & TERM_EOF), TEXT_IN_DATA);
		outb(CURSOR_HIGH, TEXT_IN_ADDR);
		outb((uint8_t)((pos >> 8) & TERM_EOF), TEXT_IN_DATA);
	}
}

/*
 * term_switch:
 * Description: Switch to a new terminal
 * Input: Next terminal to switch to 
 * Output: None
 * Effect: Switch to another terminal
 */
void term_switch(int term)
{
	int old_term;

	/* Check if terminal exist */
	if ((term < 0) || (term >= TERM_NUM))
		return;

	/* Remap video memory for safe */
	set_4KB(VIDEO, VIDEO, 0);

	/* Switch terminal */
	old_term = cur_term;
	cur_term = term;

	/* Copy video memory content into dump */
	if ((old_term >= 0) && (old_term < TERM_NUM))
		memcpy((void*)video_term[old_term], (void*)VIDEO, _4KB);

	/* Copy dump of next terminal onto video memory */
	memcpy((void*)VIDEO, (void*)video_term[cur_term], _4KB);

	/* If current process is running on current terminal, directly map video memory */
	if(current_PCB->term_num == cur_term)
		set_4KB(VIDEO, _128MB + _4MB, 3);
	/* Otherwise, map the dump instead */
	else
		set_4KB(video_term[current_PCB->term_num], _128MB + _4MB, 3);

	/* If there is no shell on the next terminal, start a new one */
	if(!terminal[term]){
		/* Clear for safe */
		clear();
		/* Clear up a space for the next shell */
		process_desc_arr[term + 1].flag = 0;
		/* Put ESP to pid0 and start up a new shell */
		asm volatile(
			"movl $0x7ffffc, %%esp\n"
			:
			:
		);
		terminal[term] = FLAG_ON;
		system_execute("shell");
	}

	/* Update cursor */
	cursor_update(cur_term);
}
