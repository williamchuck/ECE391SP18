#include "page.h"

uint32_t  page_dir_table[1024] __attribute__((alinged(4096)));
uint32_t  page_table[1024] __attribute__((aligned(4096)));

void fill_page_dir_table(){
	int i;
	for(i = 0; i < 1024; i++){
		page_dir_table[i] = 0x0;
	}
}

void fill_page_table(){
	int i;
	for(i = 0; i < 1024; i++){
		page_table[i] = 0x0;
	}
}

void set_4MB(uint32_t phys_mem, uint32_t virt_mem){
	uint32_t pde_idx = virt_mem >> 22;

	page_dir_table[pde_idx] = (phys_mem & 0xFFC00000) | EN_P | EN_R | EN_S;
	flush_TLB();
}

void set_4KB(uint32_t phys_mem, uint32_t virt_mem){
	uint32_t pde_idx, pte_idx, off_set;

	pde_idx = virt_mem >> 22;
	pte_idx = (virt_mem << 10) >> 22;
	off_set = virt_mem & 0x00000FFF;

	page_dir_table[pde_idx] = ((uint32_t)page_table & 0xFFFFF000) | EN_P | EN_R;
	page_table[pte_idx] = (phys_mem & 0xFFFFF000) | EN_P | EN_R;
	flush_TLB();
} 


void setup_page(){
	fill_page_dir_table();
	fill_page_table();

	set_4MB(0x400000, 0x400000);
	set_4KB(0xb8000, 0xb8000);
}

