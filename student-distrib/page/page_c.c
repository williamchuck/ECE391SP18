/*
 * page.c
 * Implementations of paging functions.
 * Author: Yichi Zhang.
 */
#include "page.h"

/* Page directory table */
uint32_t  page_dir[TABLE_SIZE] __attribute__((aligned(PAGE_SIZE)));
/* First page table */
uint32_t  page_table[TABLE_SIZE] __attribute__((aligned(PAGE_SIZE)));

/*
 * fill_page_dir:
 * Description: Fill page directory table with zeros
 * Input: None
 * Output: None
 * Effect: Fill page directory table with zeros
 */
void fill_page_dir(){
	int i;
	/* Set all PDE to zero*/
	for(i = 0; i < TABLE_SIZE; i++){
		page_dir[i] = 0x0;
	}
}

/*
 * fill_page_table:
 * Description: Fill page table with zeros
 * Input: None
 * Output: None
 * Effect: Fill page table with zeros
 */
void fill_page_table(){
	int i;
	/* Set all PTE to zero */
	for(i = 0; i < TABLE_SIZE; i++){
		page_table[i] = 0x0;
	}
}

/*
 * set_4MB:
 * Description: Set up 4MB page
 * Input: 4MB aligned physical memory we want to map to and a virtual memory
 *    we want to map from
 * Output: None
 * Effect: A 4MB page only accessable by kernel is setuped
 */
void set_4MB(uint32_t phys_mem, uint32_t virt_mem){
	uint32_t pde_idx;

	/* Get first 10-bits of virtual memory as the index into page directory */
	pde_idx = virt_mem >> 22;
	/* Directly map a 4MB page into page directory */
	page_dir[pde_idx] = (phys_mem & 0xFFFFF000) | EN_P | EN_R | EN_S;
	/* Flush TLB for cache */
	flush_TLB();
}

/*
 * set_4KB:
 * Description: Set up 4KB page
 * Input: 4KB aligned physical memory we want to map to and a virtual memory
 *     we want to map from
 * Output: None
 * Effect: A 4KB page only accessable by kernel is setuped
 */
void set_4KB(uint32_t phys_mem, uint32_t virt_mem){
	uint32_t pde_idx, pte_idx;

	/* Get first 10-bits of virtual memory as the index into page directory */
	pde_idx = virt_mem >> 22;
	/* Get next 10-bits of virtual memory as the index into page table */
	pte_idx = (virt_mem & 0x003FFFFF) >> 12;

	/* Set PDE points to page table addr */
	page_dir[pde_idx] = ((uint32_t)page_table & 0xFFFFF000) | EN_P | EN_R;
	/* Set PTE points to the pysical memory we want to map */
	page_table[pte_idx] = (phys_mem & 0xFFFFF000) | EN_P | EN_R;
	/* Flush TLB for cache */
	flush_TLB();
}

/*
 * setup_page:
 * Description: Set up paging for kernel page and video memory
 * Input: None
 * Output: None
 * Effect: Two page is setup: One for kernel which is 4MB,
 *     one for video memory which is 4KB
 */
void setup_page(){
	/* Initialize page directory */
	fill_page_dir();
	/* Initialize page table */
	fill_page_table();

	/* Set up a 4MB page for kernel */
	set_4MB(KERNEL_MEM, KERNEL_MEM);
	/* Set up a 4KB page for video memory */
	set_4KB(VIDEO_MEM, VIDEO_MEM);

	/* Enable page */
	enablePage();
}
