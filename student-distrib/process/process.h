#ifndef PROCESS_H_
#define PROCESS_H_

typedef PCB_block{
	file_desc_t file_desc_arr[8];
	PCB_block_t* parent_PCB;
	uint8_t* k_stack;
	uint8_t* u_stack;
} PCB_block_t;

#define current_PCB \
	((PCB_block_t *)get_current_PCB())

static inline uint32_t get_current_PCB() {
	uint32_t i, mask;
	mask = 0xFFFFE000;
	return ((uint32_t)(&i)) & mask;
}

#endif
