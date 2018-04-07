/*
 * stdin.c
 * Implementation of standard input (terminal driver for this checkpoint)
 * Author: Canlin Zhang
 */
#include "stdin.h"
#include "../fs/fs.h"


file_op_t* stdin_op = &stdin_file_op;

static file_op_t stdin_file_op = {
	.open = stdin_open,
	.read = stdin_read,
	.write = stdin_write,
	.close = stdin_close
};
/* Current code */
unsigned char cur_kbdcode;
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
	int i=0;

	/* Cast void* to unsigned char* */
	buf_ptr = (unsigned char*)buf;

	/* If input stream pointer is NULL, return error. */
	if (buf_ptr == NULL)
	{
		return -1;
	}

	/* If outside buffer size is larger than terminal buffer size, return error. */
	if (nbytes > BUF_SIZE)
	{
		return -1;
	}

	while (cur_kbdcode != KBDENP){
	    if(keypressed){
	        keypressed=0;
            if(currentchar&&i<nbytes-1){
                buf_ptr[i]=currentchar;
                i++;
            }
	    }
	}
	buf_ptr[i]='\n';
	i++;
//	/* Always clear buffer before reading */
//	for (i = 0; i < BUF_SIZE; i++)
//	{
//		term_buf[i] = TERM_EOF;
//	}
//
//	/* Initialize terminal buffer index */
//	term_buf_index = 0;
//
//	/* Block when enter is not pressed. */
//	/* Does not block outside buffer */
//	while (cur_kbdcode != KBDENP && buf_ptr == term_buf)
//	{
//
//	}
//
//	/* Read the bytes */
//	/* We assume nbytes is smaller than or equal to the size of buffer. */
//	/* We also use EOF to avoid most potential undefined behavior. */
//	for (i = 0; i < nbytes; i++)
//	{
//		/* If reached end of file, return. */
//		if (buf_ptr[i] == TERM_EOF)
//		{
//			/* Reset keycode to prevent crash in event loop */
//			if (cur_kbdcode == KBDENP)
//			{
//				cur_kbdcode = 0;
//			}
//			return (uint32_t)i;
//		}
//
//		/* Only read to buffer when input buffer is NOT keyboard */
//		/* If buffer is full, refuse new input. */
//		if ((buf_ptr != term_buf) && (term_buf_index < BUF_SIZE - 1))
//		{
//			/* Store data into buffer and increment buffer index */
//			term_buf[term_buf_index] = buf_ptr[i];
//			term_buf_index++;
//		}
//
//		/* If newline is reached, return. */
//		if (buf_ptr[i] == ASCII_NL)
//		{
//			/* Reset keycode to prevent crash in event loop */
//			if (cur_kbdcode == KBDENP)
//			{
//				cur_kbdcode = 0;
//			}
//			return (uint32_t)(i + 1);
//		}
//	}

	/* Reset keycode to prevent crash in event loop */
	if (cur_kbdcode == KBDENP)
	{
		cur_kbdcode = 0;
	}
	return i;
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
