#ifndef PROCESS_H_
#define PROCESS_H_

#include "../fs/fs.h"
#include "../types.h"
#include "../regs.h"

//0 is not used for user process, so actually only 6 proc can be ran
#define NUM_PROC 7
#define _8KB_MASK 0xFFFFE000

//Struct for PCB block
typedef struct PCB_block
 {
    regs_t* hw_context;           //saved registers for halt and process switch
    file_desc_t file_desc_arr[8]; //file descriptors for this process
    struct PCB_block* parent_PCB; //parent process's PCB
    uint32_t pid;      //process id for indexing into process_desc_arr
    uint32_t term_num; //the terminal the process is using
} PCB_block_t;

typedef struct process_desc{
	uint32_t flag;
} process_desc_t; 


/* Get current PCB pointer */
#define current_PCB \
	((PCB_block_t *)get_current_PCB())

/* Get 8KB block starting address (which is where the current PCB is located) */
static inline uint32_t get_current_PCB() {
	uint32_t i, mask;
	mask = _8KB_MASK;
	return ((uint32_t)(&i)) & mask;
}

/* Procee_desc_arr */
extern process_desc_t process_desc_arr[NUM_PROC];

/* Initialize process control */
extern void init_process();

/* Schedule the next process */
extern void schedule();
#endif
