#include "syscall.h"
#include "../x86_desc.h"

int p_num = 0;

void jump_to_user(){
	asm volatile (
		"pushl %0\n"
		"pushl %1\n"
		"pushfl\n"
		"orl $0x200, (%%esp)\n"
		"pushl %2\n"
		"pushl %3\n"
		"iret\n"
		: //no output
		:"g"(USER_DS), "g"(USER_VIRT_BOT - 4), "g"(USER_CS), "g"(entry_point)
	);
}

uint32_t current_ESP(){
	asm volatile(
		"movl (%%esp), %eax\n"
		"ret\n"
		:""
		:"g"()
	);
}
int32_t system_execute(const int8_t* file_name){
	int fd, i, size, cur_p_num;
	uint8_t* phys_addr, virt_addr, user_ESP;
	uint32_t* entry_addr;
	dentry_t dentry;

/*
	if(data_open(file_name) == -1)
		return -1;

	if(read_dentry_by_name(file_name) == -1)
		return -1;

	if(dentry.file_type != 2)
		return -1;
*/

	cur_p_num = p_num + 1;

	file_desc_t file;

	file.f_op = data_op;
	fd = file.f_op->open(file_name);
	if(fd == -1)
		return -1;

//	PCB_block_t PCB;

	current_PCB->file_desc[0].f_op = stdin_op;
	current_PCB->file_desc[0].inode = 0;
	current_PCB->file_desc[0].f_pos = 0;
	current_PCB->file_desc[0].flag = 1;


	current_PCB->file_desc[1].f_op = stdout_op;
	current_PCB->file_desc[1].inode = 0;
	current_PCB->file_desc[1].f_pos = 0;
	current_PCB->file_desc[1].flag = 1;
	

	size = get_size(file_name);
	uint8_t buf[size];
	file.f_op->read(fd, &buf, size);

	if(buf[0] != 0x7F || buf[1] != 0x45 || buf[2] != 0x4C || buf[3] != 0x46)
		return -1;

	phys_addr = 8MB + (cur_p_num * 4MB) ;
	virt_addr = 128MB;
	set_4MB(phys_addr, virt_addr);
	
	entry_addr = &(buf[24]);
	memcpy(phys_addr + 0x48000, &buf, size);

	user_ESP = phys_addr + 4MB;
	tss.esp0 = current_ESP();
	tss.esp = user_ESP;
	
	jump_to_user();

	return 0;		
}

int32_t system_halt(uint8_t status){


}
