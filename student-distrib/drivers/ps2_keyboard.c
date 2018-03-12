#include "ps2_keyboard.h"

/* Keycode set 1. Only for displaying the asciis. */
/* Further functionalites and keycode toggling will be implmented otherwise */
static unsigned char ps2kbd_set1_keycode[90] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '-', '=', 8, 9, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    10, 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 39, '`', 0, 92,
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+',
    '1', '2', '3', '0', '.', 0, 0, 0, 0, 0, 0,
};

/* Unused keycode set - Set 2 and 3 */
/* static unsigned char ps2kbd_set2_keycode[512] = {
	  0, 67, 65, 63, 61, 59, 60, 88,  0, 68, 66, 64, 62, 15, 41,117,
	  0, 56, 42, 93, 29, 16,  2,  0,  0,  0, 44, 31, 30, 17,  3,  0,
	  0, 46, 45, 32, 18,  5,  4, 95,  0, 57, 47, 33, 20, 19,  6,183,
	  0, 49, 48, 35, 34, 21,  7,184,  0,  0, 50, 36, 22,  8,  9,185,
	  0, 51, 37, 23, 24, 11, 10,  0,  0, 52, 53, 38, 39, 25, 12,  0,
	  0, 89, 40,  0, 26, 13,  0,  0, 58, 54, 28, 27,  0, 43,  0, 85,
	  0, 86, 91, 90, 92,  0, 14, 94,  0, 79,124, 75, 71,121,  0,  0,
	 82, 83, 80, 76, 77, 72,  1, 69, 87, 78, 81, 74, 55, 73, 70, 99,

	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	217,100,255,  0, 97,165,  0,  0,156,  0,  0,  0,  0,  0,  0,125,
	173,114,  0,113,  0,  0,  0,126,128,  0,  0,140,  0,  0,  0,127,
	159,  0,115,  0,164,  0,  0,116,158,  0,150,166,  0,  0,  0,142,
	157,  0,  0,  0,  0,  0,  0,  0,155,  0, 98,  0,  0,163,  0,  0,
	226,  0,  0,  0,  0,  0,  0,  0,  0,255, 96,  0,  0,  0,143,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,107,  0,105,102,  0,  0,112,
	110,111,108,112,106,103,  0,119,  0,118,109,  0, 99,104,119,  0,

	  0,  0,  0, 65, 99,
};

static unsigned char ps2kbd_set3_keycode[512] = {

	  0,  0,  0,  0,  0,  0,  0, 59,  1,138,128,129,130, 15, 41, 60,
	131, 29, 42, 86, 58, 16,  2, 61,133, 56, 44, 31, 30, 17,  3, 62,
	134, 46, 45, 32, 18,  5,  4, 63,135, 57, 47, 33, 20, 19,  6, 64,
	136, 49, 48, 35, 34, 21,  7, 65,137,100, 50, 36, 22,  8,  9, 66,
	125, 51, 37, 23, 24, 11, 10, 67,126, 52, 53, 38, 39, 25, 12, 68,
	113,114, 40, 43, 26, 13, 87, 99, 97, 54, 28, 27, 43, 43, 88, 70,
	108,105,119,103,111,107, 14,110,  0, 79,106, 75, 71,109,102,104,
	 82, 83, 80, 76, 77, 72, 69, 98,  0, 96, 81,  0, 78, 73, 55,183,

	184,185,186,187, 74, 94, 92, 93,  0,  0,  0,125,126,127,112,  0,
	  0,139,150,163,165,115,152,150,166,140,160,154,113,114,167,168,
	148,149,147,140
};

static unsigned char ps2kbd_unxlate_table[128] = {
          0,118, 22, 30, 38, 37, 46, 54, 61, 62, 70, 69, 78, 85,102, 13,
         21, 29, 36, 45, 44, 53, 60, 67, 68, 77, 84, 91, 90, 20, 28, 27,
         35, 43, 52, 51, 59, 66, 75, 76, 82, 14, 18, 93, 26, 34, 33, 42,
         50, 49, 58, 65, 73, 74, 89,124, 17, 41, 88,  5,  6,  4, 12,  3,
         11,  2, 10,  1,  9,119,126,108,117,125,123,107,115,116,121,105,
        114,122,112,113,127, 96, 97,120,  7, 15, 23, 31, 39, 47, 55, 63,
         71, 79, 86, 94,  8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 87,111,
         19, 25, 57, 81, 83, 92, 95, 98, 99,100,101,103,104,106,109,110
}; */

/* Command port and data port for ps2 keyboard (and controller, in fact) */
#define PS2_DATA_PORT                 0x60
#define PS2_CMD_PORT                  0x64
#define PS2_STATUS_PORT               0x64

/* Common return values from ps2 keyboard after given command */
/* Mainly for Debugging purposes. */
#define PS2KBD_RET_ACK                0xFA
#define PS2KBD_RET_STPASS             0xAA
#define PS2KBD_RET_STFAIL1            0xFC
#define PS2KBD_RET_STFAIL2            0xFD
#define PS2KBD_RET_ECHO               0xEE
#define PS2KBD_RET_DETECTION_NULL     0x00
#define PS2KBD_RET_DETECTION_UNKNOWN  0xFF
#define PS2KBD_RET_RESEND             0xFE

/* Common command bytes for ps2 keyboard */
/* Mainly for debugging and experiments. */
/* Attention! GETSETCODE DOES NOT WORK */
#define PS2KBD_CMD_LED                0xED
#define PS2KBD_CMD_ECHO               0xEE
#define PS2KBD_CMD_GETSETCODE         0xF0
#define PS2KBD_CMD_IDENTIFY_KBD       0xF2
#define PS2KBD_CMD_TYPEMATIC          0xF3
#define PS2KBD_CMD_ENABLE_SCAN        0xF4
#define PS2KBD_CMD_DISABLE_SCAN       0xF5
#define PS2KBD_CMD_SET_DEFAULT        0xF6
#define PS2KBD_CMD_SET_ALL_TYPEMATIC  0xF7
#define PS2KBD_CMD_SET_ALL_MAKEREA    0xF8
#define PS2KBD_CMD_SET_ALL_MAKE       0xF9
#define PS2KBD_CMD_SET_ALL_TAMR       0xFA
#define PS2KBD_CMD_SET_KEY_MR         0xFB
#define PS2KBD_CMD_SET_KEY_MAKE       0xFC
#define PS2KBD_CMD_RESEND_LAST        0xFD
#define PS2KBD_CMD_RESET_TEST         0xFF

/* Flag for toggling keys. Alt - Ctrl - CAPSL - NUML - SCRL */
static uint8_t alt_flag;
static uint8_t shift_flag;
static uint8_t capsl_flag;
static uint8_t numl_flag;
static uint8_t scroll_flag;

/*
 * ps2_keyboard_init
 *   DESCRIPTION: Initialize PS2 keyboard.
 *   ARGUMENTS: none.
 *   OUTPUT: none.
 *   RETURN VALUE: none.
 *   SIDE EFFECTS: Enable keyboard interrupt, reset key toggle flags.
 */
void ps2_keyboard_init() {
    /* Clear key toggle flags */
    alt_flag = 0x00;
    shift_flag = 0x00;
    capsl_flag = 0x00;
    numl_flag = 0x00;
    scroll_flag = 0x00;

    /* enable_irq unused, use reques_irq for installation and enabling. */
    //enable_irq(KBD_IRQ);
    request_irq(KBD_IRQ,&int_ps2kbd_c);
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
    /* Get current scan code */
    currentcode = ps2_keyboard_getscancode();
    /* If a key IS pressed, get its ascii code */
    if (currentcode != 0)
    {
      currentchar = ps2_keyboard_getchar(currentcode);
    }
    /* If this key has displable ascii code, print it out! */
    if (currentchar != 0)
    {
      printf("%c", currentchar);
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
        return ps2kbd_set1_keycode[scancode];
    }
    /* Else, return a NULL char. */
    else {
      return 0;
    }

}
