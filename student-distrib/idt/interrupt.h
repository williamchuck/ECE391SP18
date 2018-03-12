/*
 * interrupt.h
 *
 * Interrupt infrastructure
 */

#ifndef INTERRUPT_H
#define INTERRUPT_H

#define NUM_IRQ 16
#ifndef ASM

//may add other stuff
typedef struct irq_struct{
    void (*handler)();
} irq_struct_t;

//all irqs
extern irq_struct_t irqs[NUM_IRQ];

unsigned int do_IRQ();

int request_irq(unsigned int irq,
                void (*handler)());

int free_irq(unsigned int irq);

#endif /* ASM */
#endif /* INTERRUPT_H */
