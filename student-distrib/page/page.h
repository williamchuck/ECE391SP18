/*
 * page.h
 * Definitions for implementations of paging functions.
 * Author: Yichi Zhang.
 */

#ifndef PAGE_H_
#define PAGE_H_

#include "../types.h"

/* Define masks for PDE and PTE bits 0 - 9 */
#define EN_P   0x00000001 //present
#define EN_RW  0x00000002 //writable
#define EN_US  0x00000004 //user
#define EN_PWT 0x00000008 //page write through
#define EN_PCD 0x00000010 //page cache disable
#define EN_A   0x00000020 //accessed
#define EN_D   0x00000040 //dirty
#define EN_PS  0x00000080 //page size, 1 for 4MB
#define EN_G   0x00000100 //global page

/* Define table and page size for paging */
#define PAGE_SIZE 4096
#define TABLE_SIZE 1024

#define PAGE_ADDR_MASK 0xFFFFF000
#define _4MB_ADDR_MASK 0xFFC00000

/* Define start of video and kernel page */
#define KERNEL_MEM 0x400000
#define VIDEO_MEM 0xb8000

#define _1KB 0x400
#define _4KB 0x1000
#define _1MB 0x100000
#define _4MB 0x400000

/* Fill page directory table with zeros */
extern void fill_page_dir();
/* Fill page table with zeros */
extern void fill_page_table();
/* Set a 4MB page */
extern void set_4MB(uint32_t phys_mem, uint32_t virt_mem, uint8_t level);
/* Set a 4KB page */
extern void set_4KB(uint32_t phys_mem, uint32_t virt_mem, uint8_t level);

extern void free_4KB(uint32_t virt_mem);

extern void free_4MB(uint32_t virt_mem);

extern int32_t page_present(const void* virt_mem);

/* Load in new page directory table addr */
extern void flush_TLB();
/* Enable paging */
extern void enablePage();
/* Setup paging */
extern void setup_page();

#endif
