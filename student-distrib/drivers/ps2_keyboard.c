/*
 * ps2_keyboard.c
 * Implementation of PS/2 keyboard driver.
 * Author: Canlin Zhang
 */
#include "ps2_keyboard.h"

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

/* Keycode value for flag keys released */
#define LSHIFTR                       0xAA
#define RSHIFTR                       0xB6
#define ALTR                          0xB8
#define CTRLR                         0x9D

/* ON and OFF state for keyboard flags */
#define FLAG_ON						  0xFF
#define FLAG_OFF					  0x00

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

/*
 * ps2_keyboard_init
 *   DESCRIPTION: Initialize PS2 keyboard.
 *   ARGUMENTS: none.
 *   OUTPUT: none.
 *   RETURN VALUE: none.
 *   SIDE EFFECTS: Enable keyboard interrupt, reset key toggle flags.
 */
void ps2_keyboard_init() {
	/* Initialize terminal (MP3.2 only) */
	term_init();

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

	/* enable_irq unused, use reques_irq for installation and enabling. */
	//enable_irq(KBD_IRQ);
	request_irq(KBD_IRQ, &int_ps2kbd_c);
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
	/* variables for current keycode and ascii (if applicable) */
	unsigned char currentcode;
	unsigned char currentchar;
	/* Get current scan code and initialize current char */
	currentcode = ps2_keyboard_getscancode();
	currentchar = 0;
	/* If a key IS pressed, get its ascii code */
	if (currentcode != 0)
	{
		/* Process toggle flags first */
		ps2_keyboard_processflags(currentcode);

		/* Special handler for clearing screens */
		/* If ctrl + L is pressed, clear screen, reset cursor */
		if ((ctrl_flag != FLAG_OFF) && (currentcode == KBDLP))
		{
			term_init();
			return;
		}

		/* 
		 * If backspace is pressed, delete current char before cursor 
		 * and update cursor position.
		 */
		if (currentcode == KBDBKP)
		{
			term_del();
			cursor_update();
			return;
		}

		/*
		 * If enter is pressed, and the cursor is at the bottom of terminal
		 * Scroll down a line, and update cursor position.
		 */
		if ((currentcode == KBDENP) && (get_y() == VGA_HEIGHT - 1))
		{
			scroll_down();
			cursor_update();
			return;
		}

		/* Get char to be printed */
		currentchar = ps2_keyboard_getchar(currentcode);
	}
	/* If this key has displable ascii code, print it out! */
	if (currentchar != 0)
	{
		/* Call standard input to put char into buffer */
		stdin_read(TERM_IN_FD, &currentchar, 1);

		/* Echo the char out using keyboard buffer */
		/* We use terminal buffer as keyboard buffer here. */
		stdout_write(TERM_OUT_FD, &term_buf[term_buf_index - 1], 1);
		cursor_update();
	}
	/* EOI is handled by general irq handler. Hence send_eoi is NOT needed */
	//send_eoi(KBD_IRQ);
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
