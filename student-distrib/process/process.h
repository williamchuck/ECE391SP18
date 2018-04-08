#ifndef PROCESS_H_
#define PROCESS_H_

#include "../fs/fs.h"
#include "../types.h"

/*
typedef struct PCB_block{
	file_desc_t file_desc_arr[8];
	struct PCB_block* parent_PCB;
	uint8_t* k_stack;
	uint8_t* u_stack;
} PCB_block_t;
*/
typedef union PCB_block{
	uint8_t space [0x2000];
	struct {
		file_desc_t file_desc_arr[8];
		union PCB_block* parent_PCB;
		uint8_t* k_stack;
		uint8_t* u_stack;
		uint8_t padding [0];
	};
} PCB_block_t;


#define current_PCB \
	((PCB_block_t *)get_current_PCB())

static inline uint32_t get_current_PCB() {
	uint32_t i, mask;
	mask = 0xFFFFE000;
	return ((uint32_t)(&i)) & mask;
}

#endif
