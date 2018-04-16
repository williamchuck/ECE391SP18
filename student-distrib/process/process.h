#ifndef PROCESS_H_
#define PROCESS_H_

#include "../fs/fs.h"
#include "../types.h"

//0 is not used for user process, so actually only 6 proc can be ran
#define NUM_PROC 7

#define _8KB_MASK 0xFFFFE000
typedef struct PCB_block
 {
    void* halt_back_ESP;
    file_desc_t file_desc_arr[8];
    struct PCB_block* parent_PCB;
    uint32_t pid;
    uint8_t* k_stack;
    uint8_t* u_stack;
    uint8_t padding [0];
} PCB_block_t;

typedef struct process_desc{
	uint32_t flag;
} process_desc_t; 


/* Get current PCB pointer */
#define current_PCB \
	((PCB_block_t *)get_current_PCB())

/* Get 8KB block starting address */
static inline uint32_t get_current_PCB() {
	uint32_t i, mask;
	mask = _8KB_MASK;
	return ((uint32_t)(&i)) & mask;
}

/* Procee_desc_arr */
extern process_desc_t process_desc_arr[NUM_PROC];

/* Initialize process control */
extern void init_process();
#endif
