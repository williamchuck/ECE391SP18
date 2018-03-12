#include "ps2_controller.h"

/*
 * Attention! This file is for initialization of ps2 controller.
 * However, I found no need to initialize ps2 controller. So this file will not be used.
 * Also, further use of ps2 drivers would be implemented here, so don't delete this.
 */

 /*
  * ps2_controller.c
  * Initialization and (future) functionalites of PS2 controller.
  * Author: Canlin Zhang
  */

/*
 * ps2_controller_init
 *   DESCRIPTION: initialize ps2 controller
 *   ARGUMENTS: none.
 *   OUTPUT: none.
 *   RETURN VALUE: 0 for success, -1 for failure.
 *   SIDE EFFECTS: Initializes ps2 controller.
 */
int ps2_controller_init() {
    uint8_t temp;
    uint8_t dualchannel;

    /* Disable Devices */
    outb(PS2_DISABLE_TWO, PS2_CMD_PORT);
    outb(PS2_DISABLE_ONE, PS2_CMD_PORT);

    /* Flush Output Buffer */
    temp = 0x01;
    while ((temp & 0x01) != 0x00)
    {
      temp = inb(PS2_STATUS_PORT);
    }

    /* Set controller configuration byte */
    outb(PS2_READ_CONFIG, PS2_CMD_PORT);
    temp = inb(PS2_DATA_PORT);
    temp = temp & PS2_CLEAR_BIT;
    outb(PS2_WRITE_CONFIG, PS2_CMD_PORT);
    outb(temp, PS2_DATA_PORT);

    /* Perform controller self test */
    outb(PS2_SELF_TEST, PS2_CMD_PORT);
    temp = inb(PS2_DATA_PORT);
    if (temp != PS2_TEST_PASS)
    {
      return -1;
    }

    /* Determine if there are two channels */
    outb(PS2_ENABLE_TWO, PS2_CMD_PORT);
    outb(PS2_READ_CONFIG, PS2_CMD_PORT);
    temp = inb(PS2_DATA_PORT);
    if ((temp & PS2_DUAL_CHANNEL) == 0)
    {
      dualchannel = 0x01;
    }
    else
    {
      dualchannel = 0x00;
    }


    /* Enable Devices */
    outb(PS2_ENABLE_ONE, PS2_CMD_PORT);
    if (dualchannel != 0x00)
    {
      outb(PS2_ENABLE_TWO, PS2_CMD_PORT);
    }

    /* Enable Interrupt */
    outb(PS2_ENABLE_TWO, PS2_CMD_PORT);
    temp = inb(PS2_DATA_PORT);
    temp = temp | 0x03;
    outb(PS2_WRITE_CONFIG, PS2_CMD_PORT);
    outb(temp, PS2_DATA_PORT);

    return 0;
}
