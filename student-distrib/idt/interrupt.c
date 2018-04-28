/*
 * interrupt.c
 * Interrupt infrastructure implementation for IRQ - driven devices.
 * Author: Hongxuan Li.
 */

/* Include interrupt.h and PIC driver. */
#include "interrupt.h"
#include "../lib.h"
#include "../i8259.h"
#include "../process/process.h"

/* Initialize irq handler address array to NULL. */
irq_struct_t irqs[NUM_IRQ]={{0}};

/*
 * do_IRQ
 *   DESCRIPTION: execute irq handler given irq number.
 *   INPUT: pointer to register values on stack
 *   OUTPUT: none.
 *   RETURN VALUE: -1 on failure. (No handler present.)
 *                  0 on success.
 *   SIDE EFFECTS: handle irq interrupts by executing corresponding handler, and send eoi to PIC.
 */
unsigned int do_IRQ(regs_t* regs){
    /* Get IRQ number */
    unsigned int irq;
    irq = regs->orig_eax;

    current_PCB->hw_context = regs;

    if(irqs[irq].handler==NULL){
        /* Error handling for non-existant irq handler */
        printf("do_IRQ: no handler installed for IRQ%d",irq);
        /* Send EOI so other interrupts doesn't get blocked but return -1. */
        send_eoi(irq);
        return -1;
    }
    /* Call Handler */
    send_eoi(irq);
    (*(irqs[irq].handler))();
    /* Send EOI to PIC. Return 0 for success. */
    return 0;
}

/*
 * request_IRQ
 *   DESCRIPTION: install irq handler, given irq number
 *   INPUT: irq - irq number.
 *              handler - pointer to handler function to be installed.
 *   OUTPUT: none.
 *   RETURN VALUE: -1 on failure. (Handler already present / Invalid IRQ number.)
 *                  0 on success.
 *   SIDE EFFECTS: Handler for designated IRQ is installed.
 *                 Pointer to handler stored in irq struct array.
 *                 Corresponding IRQ pin on PIC is unmasked.
 */
int request_irq(unsigned int irq,
                void (*handler)()){
    unsigned long flags;
    /* Error handling for invalid irq number */
    if(irq>NUM_IRQ){
        printf("request irq: no such irq: %d", irq);
        return -1;
    }
    /* Error handling for already-installed irq handler */
    if(irqs[irq].handler!=NULL){
        printf("request irq: IRQ%d handler already installed", irq);
        return -1;
    }

    /* Disable interrupt before installing handler */
    cli_and_save(flags);
    /* Store array into handler address array */
    irqs[irq].handler=handler;
    /* Enable IRQ */
    enable_irq(irq);
    /* Enable interrupt */
    restore_flags(flags);
    return 0;
}

/*
 * free_IRQ
 *   DESCRIPTION: uninstall irq handler, given irq number
 *   INPUT: irq - irq number.
 *   OUTPUT: none.
 *   RETURN VALUE: -1 on failure. (Handler already deleted / Invalid IRQ number.)
 *                  0 on success.
 *   SIDE EFFECTS: Hanlder pointer in the corresponding entry in irq struct array is set to null.
 *                 Corresponding IRQ pin on PIC is masked.
 */
int free_irq(unsigned int irq){
    unsigned long flags;
    /* Error handling for invalid irq number */
    if(irq>NUM_IRQ){
        printf("free irq: no such irq: %d", irq);
        return -1;
    }
    /* Error handling for already-uninstalled irq handler */
    if(irqs[irq].handler==NULL){
        printf("free irq: IRQ%d handler already freed", irq);
        return -1;
    }
    /* Disable interrupt before uninstalling handler */
    cli_and_save(flags);
    /* Remove handler address from irq array */
    irqs[irq].handler=NULL;
    /* Disable irq */
    disable_irq(irq);
    /* Enable interrupt */
    restore_flags(flags);
    return 0;
}
