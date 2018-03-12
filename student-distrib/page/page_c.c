#include "page_c.h"

uint32_t  page_dir_table[1024] __attribute__((alinged(4096)));
uint32_t  page_table[1024] __attribute__((aligned(4096)));

void fill_page_dir_table(){
	int i;
	for(i = 0; i < 1024; i++){
		page_dir_table[i] = 0x00000002;
	}
}

void set_PDE(unsigned long i, unsigned long input){
	page_dir_table[i] = input;
}

void set_PTE(unsigned long i, unsigned long input){
	page_table[i] = input;
} 

void fill_page_table(){
	int i;
	for(i = 0; i < 1024; i++){
		page_table[i] = (i * 0x1000) | 3;
	}
}

void setup_page(){
	page_dir_table[0] = ((unsigned int)page_table) | 3;
}

