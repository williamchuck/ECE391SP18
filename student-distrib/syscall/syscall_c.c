#include "syscall.h"
#include "../fs/fs.h"
#include "../page/page.h"
#include "../lib.h"
#include "../x86_desc.h"
#include "../process/process.h"

int32_t get_free_pid(){
	int i;
	for(i = 0; i < 6; i++){
		if(process_desc_arr[i].flag == 0){
			//process_desc_arr[i].flag = 1;
			return i;
		}
	}

	return -1;
}

int32_t system_execute(const int8_t* file_name){
	int fd, size, child_pid, i;
	uint32_t* entry_addr;
	uint32_t child_kernel_ESP, user_ESP, phys_addr, virt_addr, pid;
	file_desc_t file;
	//get pid for child
    child_pid = get_free_pid();
    if(child_pid == -1){//check if new proc can be started
        return -1;
    }

	//read executable file
	file.f_op = data_op;
    fd = system_open(file_name);
    if(fd == -1)
        return -1;
    if(current_PCB->file_desc_arr[fd].f_op!=data_op){
        system_close(fd);
        return -1;
    }
	size = get_size(file_name);
    uint8_t buf[size];
    system_read(fd, &buf, size);

    //check executable magic
    if(buf[0] != 0x7F || buf[1] != 0x45 || buf[2] != 0x4C || buf[3] != 0x46){
        system_close(fd);
        return -1;
    }

    //mark pid used
    process_desc_arr[child_pid].flag = 1;

    //get current pid
	pid = current_PCB->pid;

	phys_addr = _8MB + (child_pid * _4MB);//user space stack and prog image
    virt_addr = _128MB;//swap all child proc on this page
    set_4MB(phys_addr, virt_addr, 3);

    entry_addr = (uint32_t*)(&(buf[24]));
    memcpy((uint8_t*)(virt_addr + 0x48000), &buf, size);


    user_ESP = virt_addr + _4MB - 8;//user stack at 128MB page+4MB, -8 so it doesnt go over page limit
    child_kernel_ESP = _8MB - (child_pid * _8KB) - 8;//kernel mode stack, ditto

    tss.esp0 = child_kernel_ESP;//esp for child's kernel mode stack

	//calculate location of child PCB
	PCB_block_t* child_PCB;
	child_PCB = (PCB_block_t*)(child_kernel_ESP - _8KB + 8);//top of the child kernel stack region is for PCB

	for(i = 0; i < 8; i++)
		child_PCB->file_desc_arr[i].flag = 0;

	child_PCB->file_desc_arr[0].f_op = stdin_op;
	child_PCB->file_desc_arr[0].inode = 0;
	child_PCB->file_desc_arr[0].f_pos = 0;
	child_PCB->file_desc_arr[0].flag = 1;

	child_PCB->file_desc_arr[1].f_op = stdout_op;
	child_PCB->file_desc_arr[1].inode = 0;
	child_PCB->file_desc_arr[1].f_pos = 0;
	child_PCB->file_desc_arr[1].flag = 1;

	child_PCB->parent_PCB = current_PCB;
	child_PCB->pid = child_pid;

	if(system_close(fd) == -1)
		return -1;
	
	return jump_to_user((uint32_t*)(*entry_addr), (uint32_t*)user_ESP, &(child_PCB->halt_back_ESP));
}

int32_t system_halt(uint8_t status){
	uint32_t pid, phys_addr, virt_addr, parent_pid;
	pid = current_PCB->pid;
	process_desc_arr[pid].flag = 0;//unset flag for pid

	parent_pid = current_PCB->parent_PCB->pid;

	phys_addr = _8MB + (parent_pid * _4MB);//get parent's userspace mem addr to remap
	virt_addr = _128MB;
	set_4MB(phys_addr, virt_addr, 3);
	

	tss.esp0 = (uint32_t)(_8MB - (parent_pid * _8KB)-8);
	
	halt_ret_exec(current_PCB->halt_back_ESP, status);

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







