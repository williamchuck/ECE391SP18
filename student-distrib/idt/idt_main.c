#include "idt_main.h"

uint32_t idt_handler_addr[IDT_ENTRY_SIZE] = {
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
  (uint32_t)int_1F_asm,
};

/* Handler address for system call */
uint32_t idt_system_call_addr = (uint32_t)int_80_asm;

void idt_set_all()
{
  int i;
  for (i = 0; i < IDT_ENTRY_SIZE; i++)
  {
    if (i == INTEL_RESERVED_9 || i == INTEL_RESERVED_15 || (i >= INTEL_RESERVED_OTHER && i < IDT_EXCEPTION_SIZE))
    {
      idt_set_entry(i, INTERRUPT_GATE_MODE, idt_handler_addr[i], KERNEL_CS, 0, 1, 0);
    }
    if (i < IDT_EXCEPTION_SIZE)
    {
      idt_set_entry(i, INTERRUPT_GATE_MODE, idt_handler_addr[i], KERNEL_CS, 0, 1, 1);
    }
    if (i == IDT_SYSTEM_CALL)
    {
      idt_set_entry(i, TRAP_GATE_MODE, idt_system_call_addr, KERNEL_CS, 3, 1, 1);
    }
  }
}
/*
 * idt_index: index
 * mode: gate type
 * handler_addr:
 * seg_selector:
 * dpl: Descriptor privilege level
 * size:
 * present: handler present
 */
void idt_set_entry(int idt_index, int mode, uint32_t handler_addr, uint16_t seg_selector, uint32_t dpl, uint32_t size, uint32_t present)
{
  if (mode == TASK_GATE_MODE)
  {
    idt[idt_index].reserved3 = 1;
    idt[idt_index].reserved2 = 0;
    idt[idt_index].reserved1 = 1;
    idt[idt_index].reserved0 = 0;
  }
  if (mode == INTERRUPT_GATE_MODE)
  {
    idt[idt_index].reserved4 = idt[idt_index].reserved4 & 0x1F;
    idt[idt_index].reserved3 = 0;
    idt[idt_index].reserved2 = 1;
    idt[idt_index].reserved1 = 1;
    idt[idt_index].reserved0 = 0;
  }
  if (mode == TRAP_GATE_MODE)
  {
    idt[idt_index].reserved4 = idt[idt_index].reserved4 & 0x1F;
    idt[idt_index].reserved3 = 1;
    idt[idt_index].reserved2 = 1;
    idt[idt_index].reserved1 = 1;
    idt[idt_index].reserved0 = 0;
  }
  SET_IDT_ENTRY(idt[idt_index], handler_addr);
  idt[idt_index].dpl = dpl;
  idt[idt_index].size = size;
  idt[idt_index].seg_selector = seg_selector;
  idt[idt_index].present = present;
}
