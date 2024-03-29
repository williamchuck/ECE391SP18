/*
 * ps2_keyboard.c
 * Implementation of PS/2 keyboard driver.
 * Author: Canlin Zhang
 */
#include "ps2_keyboard.h"
#include "stdout.h"
#include "../page/page.h"
#include "../process/process.h"

/* Three enter flags for three terminals */
uint32_t enter_flag_arr[3] = {0, 0, 0};

 /* Keycode set 1. Only for displaying the asciis. */
static unsigned char set1_code[89] = {
0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0x0A,
0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 0x27,
0x60, 0, 0x5C,
'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
'*',
0, ' ', 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0,
0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
0, 0,
};

/* Keycode set 1 for toggled keyset. Only for displaying the asciis. */
static unsigned char set1_code_toggled[89] = {
0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0x0A,
0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', 0x22,
'~', 0, '|',
'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
'*',
0, ' ', 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0,
'7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
0, 0,
};

/* Command port and data port for ps2 keyboard (and controller, in fact) */
#define PS2_DATA_PORT                 0x60
#define PS2_CMD_PORT                  0x64
#define PS2_STATUS_PORT               0x64

/* Keycode value for flag and lock keys pressed */
#define LSHIFTP                       0x2A
#define RSHIFTP                       0x36
#define ALTP                          0x38
#define CTRLP                         0x1D
#define NUMLP                         0x45
#define CAPSLP                        0x3A
#define SCRLP                         0x46
#define KBDLP                         0x26
#define KBDBKP						  0x0E
#define KBDENP						  0x1C

/* Keycode value for F1 - F3 */
#define F1P							  0x3B
#define F2P							  0x3C
#define F3P							  0x3D

/* Keycode value for flag keys released */
#define LSHIFTR                       0xAA
#define RSHIFTR                       0xB6
#define ALTR                          0xB8
#define CTRLR                         0x9D

/* Range for scancode numpad */
#define SCAN_NUMP_UP				  0x53
#define SCAN_NUMP_DOWN				  0x47
#define SCAN_NUMP_SPEC				  0x37

/* Flag for toggling keys. Alt - Ctrl - SHIFT - CAPSL - NUML - SCRL */
static uint8_t alt_flag;
static uint8_t ctrl_flag;
static uint8_t shift_flag;
static uint8_t capsl_flag;
static uint8_t numl_flag;
static uint8_t scroll_flag;

/* Flag for indicating whether a numpad key or alphabet key is pressed */
static uint8_t alpha_flag;
static uint8_t numpad_flag;

/* variables for current keycode and ascii (if applicable) */
static unsigned char currentcode;
static unsigned char currentchar;

/*
 * ps2_keyboard_init
 *   DESCRIPTION: Initialize PS2 keyboard.
 *   ARGUMENTS: none.
 *   OUTPUT: none.
 *   RETURN VALUE: none.
 *   SIDE EFFECTS: Enable keyboard interrupt, reset key toggle flags.
 */
void ps2_keyboard_init() {
	int i;

	/* Initialize terminals */
	for (i = 0; i < TERM_NUM; i++)
	{
		term_init(i);
		ps2_keyboard_clearbuf(i);
		read_flag[i] = FLAG_OFF;
	}

	cur_term = 0;

	/* Current terminal is 0. */
	//term_switch(0);

	/* Clear key toggle flags */
	alt_flag = FLAG_OFF;
	ctrl_flag = FLAG_OFF;
	shift_flag = FLAG_OFF;
	capsl_flag = FLAG_OFF;
	numl_flag = FLAG_OFF;
	scroll_flag = FLAG_OFF;

	/* Clear current keycode flags */
	alpha_flag = FLAG_OFF;
	numpad_flag = FLAG_OFF;
	enter_flag_arr[0] = FLAG_OFF;
	enter_flag_arr[1] = FLAG_OFF;
	enter_flag_arr[2] = FLAG_OFF;

	term_ready = FLAG_ON;

	/* enable_irq unused, use reques_irq for installation and enabling. */
	request_irq(KBD_IRQ, &int_ps2kbd_c);
}

/*
 * ps2_keyboard_clearbuf
 * DESCRIPTION: Clears keyboard (terminal) buffer with EOF(0xFF)
 * ARGUMENTS: none.
 * OUTPUT: none.
 * RETURN VALUE: none.
 * SIDE EFFECTS: none.
 */
void ps2_keyboard_clearbuf(int term)
{
	/* Loop var. */
	int i;

	/* Clear terminal buffer. */
	for (i = 0; i < BUF_SIZE; i++)
	{
		term_buf[term][i] = TERM_EOF;
	}

	/* Reset index. */
	term_buf_index[term] = 0;
}

/*
 * int_ps2kbd_c
 *   DESCRIPTION: C implementation for keyboard interrupt handling.
 *   ARGUMENTS: none.
 *   OUTPUT: none.
 *   RETURN VALUE: none.
 *   SIDE EFFECTS: Read current keycode and echo it onto screen (if possible).
 */
void int_ps2kbd_c() {

	/* Get current scan code and initialize current char */
	currentcode = ps2_keyboard_getscancode();

	/* If a key is pressed, get its ascii code */
	if (currentcode != 0)
	{
		/* Process toggle flags first */
		ps2_keyboard_processflags(currentcode);

		/* Handler for switching terminal and clearing screens.*/
		if (alt_flag == FLAG_ON)
		{
			switch (currentcode)
			{
			case F1P:
			{
				//alt_flag = FLAG_OFF;
				term_switch(0);
				return;
			}
			case F2P:
			{
				//alt_flag = FLAG_OFF;
				term_switch(1);
				return;
			}
			case F3P:
			{
				//alt_flag = FLAG_OFF;
				term_switch(2);
				return;
			}
			}
		}

		if(ctrl_flag == FLAG_ON)
		{
			switch (currentcode)
			{
			case KBDLP:
			{
				ctrl_flag = FLAG_OFF;
				cursor_reset(cur_term);
				clear();
				return;
			}
			}
		}

		/* Keyboard input only has real effect when stdin_read is in use. */
		if (read_flag == FLAG_OFF)
			return;

		/*
		 * If backspace is pressed, delete current char before cursor
		 * and update cursor position.
		 */
		if (currentcode == KBDBKP)
		{
			term_del(cur_term);
			cursor_update(cur_term);
			return;
		}

		/* Get char to be printed */
		currentchar = ps2_keyboard_getchar(currentcode);
	}
	/* If this key has displable ascii code, print it out! */
	if (currentchar != 0)
	{
		/* Add char into terminal(keyboard) buffer. */
		/* If current buffer is full, do nothing. */
		if (term_buf_index[cur_term] < BUF_SIZE - 1 || currentchar == ASCII_NL)
		{
			/* Add char into buffer and increment index */
			term_buf[cur_term][term_buf_index[cur_term]] = currentchar;
			term_buf_index[cur_term]++;

			/* Echo character using putc. */
			kbd_putc(currentchar);
			cursor_update(cur_term);

			/* If enter key is pressed, toggle enter flag on. */
			if (currentchar == ASCII_NL)
				enter_flag_arr[cur_term] = FLAG_ON;
		}
	}
}

/*
 * ps2_keyboard_getscancode
 *   DESCRIPTION: Get current scan code from keyboard
 *   ARGUMENTS: none.
 *   OUTPUT: none.
 *   RETURN VALUE: current scan code
 *   SIDE EFFECTS: Get current scan code from keyboard.
 */
unsigned char ps2_keyboard_getscancode() {
	unsigned char c1 = 0;
	/* ONLY READ FROM KEYBOARD REG. ONCE! */
	/* Used code on osdev wiki, but... */
	/* "The PS/2 keyboard code on OSDev is garbage." -- Andrew Sun, 3/12/2018. */
	c1 = inb(PS2_DATA_PORT);
	/* If a key is pressed, return its value */
	if (c1 > 0) {
		return c1;
	}
	return 0;
}

/*
 * ps2_keyboard_getchar
 *   DESCRIPTION: Get current ascii char from keyboard
 *   ARGUMENTS: scancode - current scancode
 *   OUTPUT: none.
 *   RETURN VALUE: current ascii char (if applicable) or NULL char (if no displable char for the key)
 *   SIDE EFFECTS: Get current ascii char from keyboard
 */
unsigned char ps2_keyboard_getchar(unsigned char scancode) {
	/* If the key pressed has a corresponding value, return it */
	if (scancode <= 90) {

		/*
		 * If Numlock is ON and a numpad key is pressed,
		 * display toggled char.
		 */
		if (numl_flag != FLAG_OFF && numpad_flag != FLAG_OFF)
		{
			return set1_code_toggled[scancode];
		}

		/*
		 * If only shift is pressed,
		 * displayed toggled char.
		 */
		if (shift_flag != FLAG_OFF && capsl_flag == FLAG_OFF)
		{
			return set1_code_toggled[scancode];
		}
		/*
		 * If shift is pressed and Capslock is ON,
		 */
		if (shift_flag != FLAG_OFF && capsl_flag != FLAG_OFF)
		{
			/* If a alphabet key is pressed, display normal lowercase char. */
			if (alpha_flag != FLAG_OFF)
			{
				return set1_code[scancode];
			}
			/* If other keys are pressed, display toggled char. */
			else
			{
				return set1_code_toggled[scancode];
			}
		}
		/*
		 * If shift is not pressed but Capslock is ON,
		 */
		if (shift_flag == FLAG_OFF && capsl_flag != FLAG_OFF)
		{
			/* If an alphabet key is pressed, displayed upper case char. */
			if (alpha_flag != FLAG_OFF)
			{
				return set1_code_toggled[scancode];
			}
			/* Otherwise display normal char. */
			else
			{
				return set1_code[scancode];
			}
		}

		/* Display normal char if no flag is present. */
		return set1_code[scancode];
	}

	/* If keycode >= 90, return a NULL char */
	return 0;
}

/*
 * ps2_keyboard_processflags
 * DESCRIPTION: Process flags according to current keycode.
 * INPUT: scancode - keycode of current pressed key
 * OUTPUT: none.
 * RETURN VALUE: none.
 * SIDE EFFECTS: Set flags of the keyboard. (Capsl, Numl, Scrl, shift, ctrl, alt, etc.)
 */
void ps2_keyboard_processflags(unsigned char scancode) {
	/* If left or right shift is pressed, set shift flag to ON */
	if ((scancode == LSHIFTP) || (scancode == RSHIFTP))
	{
		shift_flag = FLAG_ON;
	}
	/* If left or right shift is released, set shift flag to OFF */
	if ((scancode == LSHIFTR) || (scancode == RSHIFTR))
	{
		shift_flag = FLAG_OFF;
	}

	/* If Alt is pressed, set alt flag to ON */
	if (scancode == ALTP)
	{
		alt_flag = FLAG_ON;
	}
	/* If Alt is released, set alt flag to OFF */
	if (scancode == ALTR)
	{
		alt_flag = FLAG_OFF;
	}

	/* If Ctrl is pressed, set ctrl flag to ON */
	if (scancode == CTRLP)
	{
		ctrl_flag = FLAG_ON;
	}
	/* If Ctrl is released, set ctrl flag to OFF */
	if (scancode == CTRLR)
	{
		ctrl_flag = FLAG_OFF;
	}

	/* If Numlock is pressed, reverse current Numlock flag */
	if (scancode == NUMLP)
	{
		numl_flag = ~numl_flag;
	}

	/* If Capslock is pressed, reverse current Capslock flag */
	if (scancode == CAPSLP)
	{
		capsl_flag = ~capsl_flag;
	}

	/* If ScrollLock is pressed, reverse current ScrollLock flag */
	if (scancode == SCRLP)
	{
		scroll_flag = ~scroll_flag;
	}

	/* If current scancode is a numpad key, set numpad pressed flag to ON. */
	if (((scancode <= SCAN_NUMP_UP) && (scancode >= SCAN_NUMP_DOWN)) || scancode == SCAN_NUMP_SPEC)
	{
		numpad_flag = FLAG_ON;
	}
	/* Else, set numpad pressed flag to OFF */
	else
	{
		numpad_flag = FLAG_OFF;
	}

	/* If current scancode is an alphabet, set alphabet pressed flag to ON */
	if (set1_code[scancode] <= 'z' && set1_code[scancode] >= 'a')
	{
		alpha_flag = FLAG_ON;
	}
	/* Else, set alphabet pressed flag to OFF */
	else
	{
		alpha_flag = FLAG_OFF;
	}
}
