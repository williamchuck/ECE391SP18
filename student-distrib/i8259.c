/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
  /* mask all of 8259A - 1 */
  outb(0xFF, MASTER_8259_PORT + 1);
  /* mask all of 8259A - 2 */
  outb(0xFF, SLAVE_8259_PORT + 1);

  /* ICW1: select 8259A - 1 init */
  outb(ICW1, MASTER_8259_PORT);
  /* ICW2: 8259A-1 IR0-7 mapped to 0x20 - 0x27*/
  outb(ICW2_MASTER, MASTER_8259_PORT + 1);
  /* 8259A - 1 has a slave on IR2 */
  outb(ICW3_MASTER, MASTER_8259_PORT + 1);

  /* ICW1: select 8259A - 2 init */
  outb(ICW1, SLAVE_8259_PORT);
  /* ICW2: 8259A-2 IR0-7 mapped to 0x28 - 0x2f */
  outb(ICW2_SLAVE, SLAVE_8259_PORT + 1);
  /* 8259A - 2 is slave on master's IR2 */
  outb(ICW3_SLAVE, SLAVE_8259_PORT + 1);

  /* Enable All on slave and master (Can be changed to other value after testing)*/
  master_mask = 0x00;
  slave_mask = 0x00;

  /* Restore master IRQ mask */
  outb(master_mask, MASTER_8259_PORT + 1);
  /* Restore slave IRQ mask */
  outb(slave_mask, SLAVE_8259_PORT + 1);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
  if (irq_num < 0x28 && irq_num >= 0x20)
  {
    /* Calculate new mask using bitwise and */
    irq_num = irq_num - 0x20;
    master_mask = master_mask & (~(0x01 << irq_num));
    outb(master_mask, MASTER_8259_PORT + 1);
  }
  if (irq_num >= 0x28 && irq_num <= 0x2F)
  {
    /* Calculate new mask using bitwise and */
    irq_num = irq_num - 0x28;
    slave_mask = slave_mask & (~(0x01 << irq_num));
    outb(slave_mask, SLAVE_8259_PORT + 1);
  }
  return;
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
  /* If irq_num is between 0x20 and 0x27, it is using master PIC. */
  if (irq_num < 0x28 && irq_num >= 0x20)
  {
    /* Calculate new mask using bitwise or. */
    irq_num = irq_num - 0x20;
    master_mask = master_mask | (0x01 << irq_num);
    outb(master_mask, MASTER_8259_PORT + 1);
  }
  /* If irq_num is between 0x28 and 0x2F, it is using slave PIC. */
  if (irq_num >= 0x28 && irq_num <= 0x2F)
  {
    /* Calculate new mask using bitwise or. */
    irq_num = irq_num - 0x28;
    slave_mask = slave_mask | (0x01 << irq_num);
    outb(slave_mask, SLAVE_8259_PORT + 1);
  }
  return;
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
  /* If irq_num is between 0x20 and 0x27, it is using master PIC. */
  if (irq_num < 0x28 && irq_num >= 0x20)
  {
    /* Send EOI command (OCW2) to PIC */
    /* Calculate IR level (L2 to L0) in OCW2 (EOI command) for master */
    irq_num = irq_num - 0x20;
    outb(EOI | irq_num, MASTER_8259_PORT);
  }
  /* If irq_num is between 0x28 and 0x2F, it is using slave PIC. */
  if (irq_num >= 0x28 && irq_num <= 0x2F)
  {
    /* Send EOI command (OCW2) to PIC */
    /* Calculate IR level (L2 to L0) in OCW2 (EOI command) for slave*/
    irq_num = irq_num - 0x28;
    /* Send first EOI to master, IR level = ICW3_SLAVE */
    outb(EOI | ICW3_SLAVE, MASTER_8259_PORT);
    /* Send second EOI to slave, IR level calculated above. */
    outb(EOI | irq_num, SLAVE_8259_PORT);

  }
  return;
}
