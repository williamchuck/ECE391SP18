#ifndef PAGE_H_
#define PAGE_H_

#include "../types.h"

/* Define masks for PDE and PTE bits 0 - 9 */
#define EN_P 0x00000001
#define EN_R 0x00000002
#define EN_U 0x00000004
#define EN_W 0x00000008
#define EN_D 0x00000010
#define EN_A 0x00000020
#define EN_S 0x00000080
#define EN_G 0x00000100

extern void fill_page_dir_table();
extern void set_4MB(uint32_t phys_mem, uint32_t virt_mem);
extern void fill_page_table();
extern void setPTE(uint32_t phys_mem, uint32_t virt_mem);
extern flush_TLB();

#endif
