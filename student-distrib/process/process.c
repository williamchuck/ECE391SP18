#include "process.h"

/* Process_desc_arr to contorl processes */
process_desc_t process_desc_arr[6];

void init_process(){
	/* Initilize process_desc_arr */
	int i;
	for(i = 0; i < 6; i++)
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
