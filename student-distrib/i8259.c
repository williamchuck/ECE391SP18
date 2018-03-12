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
  //mask all interrupt when setting up
  outb(0xFF, MASTER_8259_DATA);
  outb(0xFF, SLAVE_8259_DATA);

  outb(ICW1, MASTER_8259_PORT);
  outb(ICW2_MASTER, MASTER_8259_DATA);
  outb(ICW3_MASTER, MASTER_8259_DATA);
  outb(ICW4, MASTER_8259_DATA);

  outb(ICW1, SLAVE_8259_PORT);
  outb(ICW2_SLAVE, SLAVE_8259_DATA);
  outb(ICW3_SLAVE, SLAVE_8259_DATA);
  outb(ICW4, SLAVE_8259_DATA);


  outb(0xFF, MASTER_8259_DATA);
  outb(0xFF, SLAVE_8259_DATA);
  enable_irq(2);//enable slave cascade

}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
  if (irq_num >= 0x00 && irq_num <= 0x07)
  {
    master_mask = master_mask & (~(0x01 << irq_num));
    outb(master_mask, MASTER_8259_DATA);
  }
  else if (irq_num >= 0x08 && irq_num <= 0x0F)
  {
    irq_num = irq_num - 0x08;
    slave_mask = slave_mask & (~(0x01 << irq_num));
    outb(slave_mask, SLAVE_8259_DATA);
  }
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
  if (irq_num >= 0x00 && irq_num <= 0x07)
  {
    master_mask = master_mask | (0x01 << irq_num);
    outb(master_mask, MASTER_8259_DATA);
  }
  else if (irq_num >= 0x08 && irq_num <= 0x0F)
  {
    irq_num = irq_num - 0x08;
    slave_mask = slave_mask | (0x01 << irq_num);
    outb(slave_mask, SLAVE_8259_DATA);
  }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
  if (irq_num >= 0x00 && irq_num <= 0x07)
  {
    outb(EOI + irq_num, MASTER_8259_PORT);
  }
  else if (irq_num >= 0x08 && irq_num <= 0x0F)
  {
    outb(EOI + irq_num - 0x08, SLAVE_8259_PORT);
    outb(EOI + ICW3_SLAVE, MASTER_8259_PORT);
  }
}
