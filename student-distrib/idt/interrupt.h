/*
 * interrupt.h
 * Interrupt infrastructure for IRQ - driven devices.
 * Author: Hongxuan Li.
 */

#ifndef INTERRUPT_H
#define INTERRUPT_H

#define NUM_IRQ 16
#ifndef ASM

#include "../regs.h"
/*
 * Until MP3.1, only handler is in the struct.
 * More members pending.
 */
typedef struct irq_struct{
    void (*handler)();
} irq_struct_t;


/*
 * IRQ handler implementation.
 */
unsigned int do_IRQ(regs_t* regs);

/*
 * Setup IRQ handler for specific IRQ number.
 */
int request_irq(unsigned int irq,
                void (*handler)());

/*
 * Delete IRQ handler for specific IRQ number.
 */
int free_irq(unsigned int irq);

#endif /* ASM */
#endif /* INTERRUPT_H */
