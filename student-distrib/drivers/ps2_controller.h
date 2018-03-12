/*
 * Attention! This file is for initialization of ps2 controller.
 * However, I found no need to initialize ps2 controller. So this file will not be used.
 * Also, further use of ps2 drivers would be implemented here, so don't delete this.
 */
/*
 * ps2_controller.h
 * Initialization and (future) functionalites of PS2 controller.
 * Author: Canlin Zhang
 */
#ifndef PS2_CONTROLLER_H
#define PS2_CONTROLLER_H

#include "../types.h"
#include "../lib.h"

/* PS2 controller data ports */
#define PS2_DATA_PORT                 0x60
#define PS2_CMD_PORT                  0x64
#define PS2_STATUS_PORT               0x64

/* PS2 commands */
#define PS2_DISABLE_ONE               0xAD
#define PS2_DISABLE_TWO               0xA7
#define PS2_SELF_TEST                 0xAA
#define PS2_TEST_PASS                 0x55
#define PS2_TEST_FAIL                 0xFC
#define PS2_READ_CONFIG               0x20
#define PS2_WRITE_CONFIG              0x60
#define PS2_ENABLE_ONE                0xAE
#define PS2_ENABLE_TWO                0xA8
#define PS2_RESET_PASS                0xFA
#define PS2_CLEAR_BIT                 0xBC
#define PS2_DUAL_CHANNEL              0x20

/* Function to initialize ps2 controller (not needed)*/
int ps2_controller_init();

#endif
