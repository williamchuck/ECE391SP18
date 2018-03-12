/*
 * interrupt.c
 *
 *  Interrupt infrastructure
 */

#include "interrupt.h"
#include "../lib.h"
#include "../i8259.h"

irq_struct_t irqs[NUM_IRQ]={{0}};

unsigned int do_IRQ(unsigned int irq){
    if(irqs[irq].handler==NULL){
        if(irq)printf("do_IRQ: no handler installed for IRQ%d",irq);
        send_eoi(irq);
        return -1;
    }
    (*(irqs[irq].handler))();
    send_eoi(irq);
    return 0;
}

int request_irq(unsigned int irq,
                void (*handler)()){
    unsigned long flags;
    if(irq>NUM_IRQ){
        printf("request irq: no such irq: %d", irq);
        return -1;
    }
    if(irqs[irq].handler!=NULL){
        printf("request irq: IRQ%d handler already installed", irq);
        return -1;
    }

    cli_and_save(flags);
    irqs[irq].handler=handler;
    enable_irq(irq);
    restore_flags(flags);
    return 0;
}

int free_irq(unsigned int irq){
    unsigned long flags;
    if(irq>NUM_IRQ){
        printf("free irq: no such irq: %d", irq);
        return -1;
    }
    if(irqs[irq].handler==NULL){
        printf("free irq: IRQ%d handler already freed", irq);
        return -1;
    }
    cli_and_save(flags);
    irqs[irq].handler=NULL;
    disable_irq(irq);
    restore_flags(flags);
    return 0;
}
