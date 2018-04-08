/*
 * idt_main.c
 * implementation of idt initialization
 * Authors: Hongxuan Li, Canlin Zhang
 */

#include "idt_main.h"

/*
 * Array of address of system exception handler functions.
 * Functions defined in idt_handler_asm.h
 */
uint32_t idt_handler_addr[0x20] = {
    (uint32_t)int_0_asm,
    (uint32_t)int_1_asm,
    (uint32_t)int_2_asm,
    (uint32_t)int_3_asm,
    (uint32_t)int_4_asm,
    (uint32_t)int_5_asm,
    (uint32_t)int_6_asm,
    (uint32_t)int_7_asm,
    (uint32_t)int_8_asm,
    (uint32_t)int_9_asm,
    (uint32_t)int_A_asm,
    (uint32_t)int_B_asm,
    (uint32_t)int_C_asm,
    (uint32_t)int_D_asm,
    (uint32_t)int_E_asm,
    (uint32_t)int_F_asm,
    (uint32_t)int_10_asm,
    (uint32_t)int_11_asm,
    (uint32_t)int_12_asm,
    (uint32_t)int_13_asm,
    (uint32_t)int_14_asm,
    (uint32_t)int_15_asm,
    (uint32_t)int_16_asm,
    (uint32_t)int_17_asm,
    (uint32_t)int_18_asm,
    (uint32_t)int_19_asm,
    (uint32_t)int_1A_asm,
    (uint32_t)int_1B_asm,
    (uint32_t)int_1C_asm,
    (uint32_t)int_1D_asm,
    (uint32_t)int_1F_asm
};

/*
 * Array of address of IRQ handler functions.
 * Functions defined in idt_handler_asm.h
 */
uint32_t irq_handler_addr[0x10] = {
    (uint32_t)int_20_asm,
    (uint32_t)int_21_asm,
    (uint32_t)int_22_asm,
    (uint32_t)int_23_asm,
    (uint32_t)int_24_asm,
    (uint32_t)int_25_asm,
    (uint32_t)int_26_asm,
    (uint32_t)int_27_asm,
    (uint32_t)int_28_asm,
    (uint32_t)int_29_asm,
    (uint32_t)int_2A_asm,
    (uint32_t)int_2B_asm,
    (uint32_t)int_2C_asm,
    (uint32_t)int_2D_asm,
    (uint32_t)int_2E_asm,
    (uint32_t)int_2F_asm
};

/* Handler address for system call */
uint32_t idt_system_call_addr = (uint32_t)syscall_asm;

/*
 * idt_set_all()
 *   DESCRIPTION: Set idt entry 0x00 to 0x20 (Syetem exception)
 *                              0x21 to 0x2F (IRQ handlers)
 *   ARGUMENTS: none.
 *   OUTPUT: none.
 *   RETURN VALUE: none.
 *   SIDE EFFECTS: All exception idt entries and IRQ idt entries will be initialized.
 */
void idt_set_all() {
    int i;
    /* Set idt entries 0x00 to 0x20 */
    for (i = 0; i < 0x20; i++) {
        /* If entry is intel reserved, set idt entry with interrupt gate mode */
        /* Present flag = 1, size = 1 (32-bit), previlege = 0 (system) */
        if (i == INTEL_RESERVED_9 || i == INTEL_RESERVED_15
                || (i >= INTEL_RESERVED_OTHER && i < IDT_EXCEPTION_SIZE)) {
            idt_set_entry(i, INTERRUPT_GATE_MODE, idt_handler_addr[i],
                    KERNEL_CS, 0, 1, 1);
        }
        /* For other system exceptions, also set idt entry with interrupt gate mode */
        /* Present flag = 1, size = 1 (32-bit), previlege = 0 (system) */
        if (i < IDT_EXCEPTION_SIZE) {
            idt_set_entry(i, INTERRUPT_GATE_MODE, idt_handler_addr[i],
                    KERNEL_CS, 0, 1, 1);
        }
    }
    /* entries 0x30 to 0x7F will not be used. Set their present flag to 0. */
    for (i = 0x30; i <= 0x7F; i++) {
        idt[i].present = 0;
    }

    /* Set system call entry. (0x80) */
    /* Present flag = 1, size = 1 (32-bit), previlege = 3 (user) */
    idt_set_entry(0x80, TRAP_GATE_MODE, idt_system_call_addr, KERNEL_CS, 3, 1, 1);

    /* entries 0x80 to 0xFF will not be used. Set their present flag to 0. */
    for (i = 0x81; i <= 0xFF; i++) {
        idt[i].present = 0;
    }

    /* Set IRQ handlers (0x20 to 0x2F) */
    for (i = 0x20; i <= 0x2F; i++) {
        /* Present flag = 1, size = 1 (32-bit), previlege = 0 (system)*/
        idt_set_entry(i, INTERRUPT_GATE_MODE, irq_handler_addr[i-0x20], KERNEL_CS, 0, 1, 1);
    }
}
/*
 * idt_set_entry
 *   DESCRIPTION: Set a specific idt entry with given index and information.
 *   ARGUMENTS:   idt_index - index of idt entry
 *                mode - gate type, (INT, TASK or TRAP).
 *                handler_addr - address of handler function
 *                seg_selector - Segment selector
 *                dpl -  Descriptor privilege level, maximum previlege level of caller
 *                size - size of idt entry (32 bit - 1, 16 bit - 0)
 *                present - flag determining whether handler is present. NO - 0, YES - 1
 *   OUTPUT: none.
 *   RETURN VALUE: none.
 *   SIDE EFFECTS: Set a specific idt entry.
 */
void idt_set_entry(int idt_index, int mode, uint32_t handler_addr,
        uint16_t seg_selector, uint32_t dpl, uint32_t size, uint32_t present) {
    /* For task mode, set reserved bits to 0p101 - reserved */
    if (mode == TASK_GATE_MODE) {
        idt[idt_index].reserved4 = 0;
        idt[idt_index].reserved3 = 1;
        idt[idt_index].reserved2 = 0;
        idt[idt_index].reserved1 = 1;
        idt[idt_index].reserved0 = 0;
    }
    /* For interrupt mode, set reserved bits to 0p110 - reserved */
    if (mode == INTERRUPT_GATE_MODE) {
        idt[idt_index].reserved4 = 0;
        idt[idt_index].reserved3 = 0;
        idt[idt_index].reserved2 = 1;
        idt[idt_index].reserved1 = 1;
        idt[idt_index].reserved0 = 0;
    }
    /* For trap mode, set reserved bits to 0p111 - reserved */
    if (mode == TRAP_GATE_MODE) {
        idt[idt_index].reserved4 = 0;
        idt[idt_index].reserved3 = 1;
        idt[idt_index].reserved2 = 1;
        idt[idt_index].reserved1 = 1;
        idt[idt_index].reserved0 = 0;
    }

    /* Set IDT handler address */
    SET_IDT_ENTRY(idt[idt_index], handler_addr);

    /* Set previlege, size, segment selector and present flag. */
    idt[idt_index].dpl = dpl;
    idt[idt_index].size = size;
    idt[idt_index].seg_selector = seg_selector;
    idt[idt_index].present = present;
}
