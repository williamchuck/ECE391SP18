#include "process.h"
#include "../drivers/stdout.h"
#include "../syscall/syscall.h"
#include "../page/page.h"
#include "../x86_desc.h"

/* Process_desc_arr to contorl processes */
process_desc_t process_desc_arr[NUM_PROC];


/*
 * get_next_schedule:
 * Description: Get the pid of the next schedulable process
 * Input: None
 * Output: pid of the next process to schedule, -1 on fail
 * Effect: None
 */
int32_t get_next_schedule(){
	int i;
	/* Start from the next pid to current pid, search for pid with flag 2 */
	for(i = current_PCB->pid + 1; i != current_PCB->pid; i = (i + 1) % NUM_PROC){
		/* Ignore the pid0 for safe */
		if(i == 0)
			continue;

		/* If we find it, return pid */
		if(process_desc_arr[i].flag == 2)
			return i;
	}

	/* Cannot find a process to schedule, return -1 */
	return -1;
}

/*
 * init_process:
 * Description: Initialize the process_desc_arr for furture use
 * Input: None
 * Output: None
 * Effect: Initialize the process_desc_arr
 */
void init_process(){
	/* Initilize process_desc_arr */
	int i;
	for(i = 0; i < NUM_PROC; i++)
		process_desc_arr[i].flag = 0;

	/* use pid 0 as sentinel process */
	process_desc_arr[0].flag = 3;

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

/*
 * schedule:
 * Description: Schedule the next process
 * Input: None
 * Output: None
 * Effect: Schedule the next process to run
 */
void schedule(){
	/* Initialize variables */
	uint32_t phys_addr, virt_addr, next_ESP, next_pid;
	PCB_block_t* next_PCB;
	
	/* Get pid of next process to schedule, return on fail */
	next_pid = get_next_schedule();
	if(next_pid == -1)
		return;

	/* Get PCB of the next process and set TSS */
	next_ESP = _8MB - (next_pid * _8KB);
	next_PCB = (PCB_block_t*)((next_ESP - 1) & _8KB_MASK);
	tss.esp0 = (uint32_t)(_8MB - (next_pid * _8KB));

	
	/* Remap user program mem for next program */
	phys_addr = _8MB + (next_pid * _4MB);
	virt_addr = _128MB;
	set_4MB(phys_addr, virt_addr, 3);

	/* Remap video mem for next program */
	if(next_PCB->term_num == cur_term)
		set_4KB(VIDEO_MEM, _128MB + _4MB, 3);
	else 
		set_4KB(video_term[next_PCB->term_num], _128MB + _4MB, 3);	

	/* Mark process_desc_arr flags */
	process_desc_arr[current_PCB->pid].flag = 2;
	process_desc_arr[next_pid].flag = 1;

    	/* Context switch to user */
    	asm volatile(
		"movl %0, %%esp\n"
        	"jmp return_to_user\n"
		:
		: "g"(next_PCB->hw_context)
  	);
}
