/*
 * stdin.c
 * Implementation of standard input (terminal driver for this checkpoint)
 * Author: Canlin Zhang
 */
#include "stdin.h"

/*
 * stdin_open
 * DESCRIPTION: "open" system call handler for standard input
 * INPUT: filename - file to be opened.
 * OUTPUT: none.
 * RETURN VALUE: 0 on success, -1 on failure
 * NOTES: More functionality to be implemented. This function now does nothing and returns 0.
 */
int32_t stdin_open(const uint8_t* filename)
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
 *		  buf - incoming data stream (buffer) to be read
 *		  nbytes - number of bytes to be read
 * OUTPUT: none.
 * RETURN VALUE: number of bytes read on success, -1 on failure.
 * SIDE EFFECTS: Data will be stored into buffer. Return value affects sys. call.
 */
int32_t stdin_read(int32_t fd, void* buf, int32_t nbytes)
{
	/* Pointer to data stream to be read */
	unsigned char* buf_ptr;
	
	/* Loop var. */
	int i, j;

	/* Cast void* to unsigned char* */
	buf_ptr = (unsigned char*)buf;

	/* If input stream pointer is NULL, return error. */
	if (buf_ptr == NULL)
	{
		return -1;
	}

	/* Block when enter is not pressed. */
	while (currentcode != KBDENP)
	{

	}

	/* Read the bytes */
	/* We assume nbytes is smaller than or equal to the size of buffer. */
	/* We also use EOF to avoid most potential undefined behavior. */
	for (i = 0; i < nbytes; i++)
	{
		/* If buffer is full, clear current buffer. */
		/* NOT to be used for file stream... */
		if (term_buf_index == BUF_SIZE)
		{
			for (j = 0; j < BUF_SIZE; j++)
			{
				term_buf[j] = TERM_EOF;
			}
			term_buf_index = 0;
		}

		/* If reached end of file, return. */
		if (buf_ptr[i] == TERM_EOF)
		{
			return (uint32_t)i;
		}
		
		/* Store data into buffer and increment buffer index */
		term_buf[term_buf_index] = buf_ptr[i];
		term_buf_index++;

		/* If newline is reached, return. */
		if (buf_ptr[i] == ASCII_NL)
		{
			return (uint32_t)(i + 1);
		}
	}

	return nbytes;
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
int32_t stdin_write(int32_t fd, const void* buf, int32_t nbytes)
{
	return -1;
}
