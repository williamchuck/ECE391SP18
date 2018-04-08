#include "syscall.h"
#include "../fs/fs.h"
#include "../page/page.h"
#include "../lib.h"
#include "../x86_desc.h"
#include "../process/process.h"

int pid = 0;

int32_t system_execute(const int8_t* file_name){
	int fd, size;
	uint32_t* entry_addr;
	uint32_t current_ESP, user_ESP, phys_addr, virt_addr;

	file_desc_t file;

	file.f_op = data_op;
	fd = file.f_op->open(file_name);
	if(fd == -1)
		return -1;

	current_PCB->file_desc_arr[0].f_op = stdin_op;
	current_PCB->file_desc_arr[0].inode = 0;
	current_PCB->file_desc_arr[0].f_pos = 0;
	current_PCB->file_desc_arr[0].flag = 1;


	current_PCB->file_desc_arr[1].f_op = stdout_op;
	current_PCB->file_desc_arr[1].inode = 0;
	current_PCB->file_desc_arr[1].f_pos = 0;
	current_PCB->file_desc_arr[1].flag = 1;
	

	size = get_size(file_name);
	uint8_t buf[size];
	file.f_op->read(fd, &buf, size);

	//if(buf[0] != 0x464C457F)
	//	return -1;

	if(buf[0] != 0x7F || buf[1] != 0x45 || buf[2] != 0x4C || buf[3] != 0x46)
		return -1;

	phys_addr = _8MB + (pid * _4MB) ;
	virt_addr = _128MB;
	set_4MB(phys_addr, virt_addr, 3);
	
	entry_addr = (uint32_t*)(&(buf[24]));
	memcpy((uint8_t*)(virt_addr + 0x48000), &buf, size);

	user_ESP = virt_addr + _4MB;
	current_ESP = _8MB - (pid * _8KB);

	pid++;
	tss.esp0 = current_ESP;
	
	jump_to_user((uint32_t*)(*entry_addr), (uint32_t*)(_128MB + _4MB - 8));


	return 0;		
}

int32_t system_halt(uint8_t status){
	return -1;
}


int32_t system_open(const int8_t* fname){
	dentry_t dentry;
	file_desc_t file;
	

	read_dentry_by_name(fname, &dentry);
	
	if(dentry.file_type == 0)
		file.f_op = rtc_op;
	else if(dentry.file_type == 1)
		file.f_op = dir_op;
	else if(dentry.file_type == 2)
		file.f_op = data_op;
	else
		return -1;

	return file.f_op->open(fname);
}

int32_t system_read(int32_t fd, void* buf, uint32_t size){
	return current_PCB->file_desc_arr[fd].f_op->read(fd, buf, size);
}

int32_t system_write(int32_t fd, const void* buf, uint32_t size){
	return current_PCB->file_desc_arr[fd].f_op->write(fd, buf, size);	
}

int32_t system_close(int32_t fd){
	return current_PCB->file_desc_arr[fd].f_op->close(fd);	
}







