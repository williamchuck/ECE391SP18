/*
 * stdin.c
 * Implementation of standard input (terminal driver for this checkpoint)
 * Author: Canlin Zhang
 */
#include "stdin.h"
#include "../fs/fs.h"

/* Standard input file operation structure. */
static file_op_t stdin_file_op = {
	.open = stdin_open,
	.read = stdin_read,
	.write = stdin_write,
	.close = stdin_close
};

/* Pointer to file operation structure. */
file_op_t* stdin_op = &stdin_file_op;

/*
 * stdin_open
 * DESCRIPTION: "open" system call handler for standard input
 * INPUT: filename - file to be opened.
 * OUTPUT: none.
 * RETURN VALUE: 0 on success, -1 on failure
 * NOTES: More functionality to be implemented. This function now does nothing and returns 0.
 */
int32_t stdin_open(const int8_t* filename)
{
	return 0;
}

/*
 * stdin_close
 * DESCRIPTION: "close" system call handler for standard input.
 * INPUT: fd - file descriptor.
 * OUTPUT: none.
 * RETURN VALUE: 0 on success. -1 on failure.
 * SIDE EFFECTS: Return value affects system call.
 * NOTES: More functionality to be implemented. This function now does nothing and returns 0.
 */
int32_t stdin_close(int32_t fd)
{
	return 0;
}

/*
 * stdin_read
 * DESCRIPTION: "read" system call handler for standard input
 * INPUT: fd - file descriptor
 *		  buf - target buffer to be stored into
 *		  nbytes - number of bytes to be read
 * OUTPUT: none.
 * RETURN VALUE: number of bytes read on success, -1 on failure.
 * SIDE EFFECTS: Data will be stored into target buffer. Return value affects sys. call.
 */
int32_t stdin_read(int32_t fd, void* buf, uint32_t nbytes)
{
	/* Pointer to data stream to be read */
	unsigned char* buf_ptr;
	
	/* Loop var. */
	int i=0;

	/* Cast void* to unsigned char* */
	buf_ptr = (unsigned char*)buf;

	/* If input stream pointer is NULL, return error. */
	if (buf_ptr == NULL)
	{
		return -1;
	}

	/* Hold for enter key press. */
	while (!enter_flag)
	{

	}
	
	/* Always clear target buffer with NULL */
	for (i = 0; i < nbytes; i++)
	{
		buf_ptr[i] = 0x00;
	}

	/* Read from internal buffer. */
	for (i = 0; i < BUF_SIZE; i++)
	{
		/* If end of file, return. */
		if (term_buf[i] == TERM_EOF)
		{
			/* Reset enter flag to avoid potential race conditions. */
			enter_flag = 0;

			/* Clear buffer and return. */
			ps2_keyboard_clearbuf();
			return i;
		}

		/* If target buffer size exceeds local size, return. */
		if (i >= nbytes)
		{
			/* Reset enter flag to avoid potential race conditions. */
			enter_flag = 0;

			/* Clear buffer and return. */
			ps2_keyboard_clearbuf();
			return nbytes;
		}

		/* Copy terminal buffer into target buffer. */
		buf_ptr[i] = term_buf[i];
	}

	/* Reset enter flag to avoid potential race conditions. */
	enter_flag = 0;

	/* Clear buffer and return. */
	ps2_keyboard_clearbuf();
	return BUF_SIZE;
}

/*
* stdin_write
* DESCRIPTION: "write" system call handler for standard input
* INPUT: fd - file descriptor
*		  buf - data stream to be output (displayed onto terminal)
*		  nbytes - number of bytes to be displayed onto terminal
* OUTPUT: none.
* RETURN VALUE: number of bytes displayed on success. -1 on failure.
* SIDE EFFECTS: none.
* NOTE: System is NOT supposed to call this. Return error only.
*/
int32_t stdin_write(int32_t fd, const void* buf, uint32_t nbytes)
{
	return -1;
}
