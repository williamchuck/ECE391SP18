/*
 * page.h
 * Definitions for implementations of paging functions.
 * Author: Yichi Zhang.
 */

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

/* Define table and page size for paging */
#define PAGE_SIZE 4096
#define TABLE_SIZE 1024

/* Define start of video and kernel page */
#define KERNEL_MEM 0x400000
#define VIDEO_MEM 0xb8000

/* Fill page directory table with zeros */
extern void fill_page_dir();
/* Fill page table with zeros */
extern void fill_page_table();
/* Set a 4MB page */
extern void set_4MB(uint32_t phys_mem, uint32_t virt_mem, uint8_t level);
/* Set a 4KB page */
extern void set_4KB(uint32_t phys_mem, uint32_t virt_mem, uint8_t level);
/* Load in new page directory table addr */
extern void flush_TLB();
/* Enable paging */
extern void enablePage();
/* Setup paging */
extern void setup_page();

#endif
