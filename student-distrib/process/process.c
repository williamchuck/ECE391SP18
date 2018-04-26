#include "process.h"
#include "../syscall/syscall.h"
#include "../page/page.h"
#include "../x86_desc.h"

/* Process_desc_arr to contorl processes */
process_desc_t process_desc_arr[NUM_PROC];

void init_process(){
	/* Initilize process_desc_arr */
	int i;
	for(i = 0; i < NUM_PROC; i++)
		process_desc_arr[i].flag = 0;

	/* use pid 0 as sentinel process */
	process_desc_arr[0].flag = 1;

	/* Clear up file desc array for the first process */
	for(i = 0; i < 8; i++)
	    current_PCB->file_desc_arr[i].flag = 0;

	/* Initialize first process */
	current_PCB->parent_PCB = NULL;
	current_PCB->pid = 0;
	current_PCB->file_desc_arr[0].f_op = stdin_op;
    	current_PCB->file_desc_arr[0].inode = 0;
    	current_PCB->file_desc_arr[0].f_pos = 0;
    	current_PCB->file_desc_arr[0].flag = 1;

    	current_PCB->file_desc_arr[1].f_op = stdout_op;
    	current_PCB->file_desc_arr[1].inode = 0;
    	current_PCB->file_desc_arr[1].f_pos = 0;
    	current_PCB->file_desc_arr[1].flag = 1;
}

void schedule(int pid){
	/* Initialize variables */
	uint32_t phys_addr, virt_addr, user_ESP, kernel_ESP;
	PCB_block_t* next_PCB;

	/* Remap user program mem for next program */
	phys_addr = _8MB + (pid * _4MB);
	virt_addr = _128MB;
	set_4MB(phys_addr, virt_addr, 3);
	
	/* Remap video mem for next program */
	set_4KB(VIDEO_MEM, _128MB + _4MB, 3);
	
	/* Restore registers for next program */
	
	/* Restore TSS for next program */
	tss.esp0 = kernel_ESP;
}
