#include "syscall.h"
#include "../fs/fs.h"
#include "../page/page.h"
#include "../lib.h"
#include "../x86_desc.h"
#include "../process/process.h"

/*
 * get_free_pid:
 * Description: Get a free pid for child process
 * Input: None
 * Output: pid for child process
 * Effect: None
 */
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

/*
 * system_execute:
 * Description: Execute system call kernel function
 * Input: File name of the file want to execute
 * Output: -1 on fail, 0 on success
 * Effect: Execute the file 
 */
int32_t system_execute(const int8_t* file_name){
    int fd, size, child_pid, i;
    void* entry_addr;
    uint32_t child_kernel_ESP, user_ESP, phys_addr, virt_addr, pid;
    file_desc_t file;

    /* Get pid for child */
    child_pid = get_free_pid();
    /* Check if pid is valid */
    if(child_pid == -1){
        return -1;
    }

    /* open file */
    file.f_op = data_op;
    fd = system_open(file_name);
    if(fd == -1)
        return -1;
    if(current_PCB->file_desc_arr[fd].f_op != data_op){
        system_close(fd);
        return -1;
    }


    /* Get current pid */
    pid = current_PCB->pid;
    /* Get file size before switching page since the string wouldn't be accessible once page is switched */
    size = get_size(file_name);

    /* Set up correct memory mapping for child process */
    phys_addr = _8MB + (child_pid * _4MB);
    virt_addr = _128MB;
    set_4MB(phys_addr, virt_addr, 3);

    /* Load user program */
    uint8_t* buf=(uint8_t*)(virt_addr + OFFSET);
    system_read(fd, buf, size);

    /* Check if file is executable */
    if(buf[0] != EXE_MAGIC_0 || buf[1] != EXE_MAGIC_1 || buf[2] != EXE_MAGIC_2 || buf[3] != EXE_MAGIC_3){
	/* Close file */
        system_close(fd);
	/* Remap memory back to current process */
        set_4MB(_8MB + (pid * _4MB), virt_addr, 3);
        return -1;
    }

    /* Mark child pid as used */
    process_desc_arr[child_pid].flag = 1;

    /* Get the address of entry addr */
    entry_addr = *((void**)(&(buf[ENTRY_POS])));

    /* Set up user esp at the bottom of the user page, -8 so it doesnt go over page limit */
    user_ESP = virt_addr + _4MB - 8;
    /* Set up child process kernel esp at the bottom of the 8KB block */
    child_kernel_ESP = _8MB - (child_pid * _8KB) - 8;

    /* Set TSS esp0 to child process kernel stack */
    tss.esp0 = child_kernel_ESP;

	/* Calculate location of child PCB */
	PCB_block_t* child_PCB;
	child_PCB = (PCB_block_t*)(child_kernel_ESP - _8KB + 8);

	/* Initialize file_desc_arr for child process */
	for(i = 0; i < 8; i++)
		child_PCB->file_desc_arr[i].flag = 0;

	/* Initialize child process control block */
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

	/* Close the file */
	if(system_close(fd) == -1)
		return -1;
	
	/* Jump to user space */
	return jump_to_user(entry_addr, (uint32_t*)user_ESP, &(child_PCB->halt_back_ESP));
}

/*
 * system_halt:
 * Description: Halt system call kernel function
 * Input: Return value to user space
 * Output: Always -1 because it should not return
 * Effect: Halt the file
 */
int32_t system_halt(uint8_t status){
	uint32_t pid, phys_addr, virt_addr, parent_pid;
	pid = current_PCB->pid;

	/* Unset flag for current pid */
	process_desc_arr[pid].flag = 0;

	/* Get parent pid */
	parent_pid = current_PCB->parent_PCB->pid;

	/* Get parent's userspace mem addr to remap */
	phys_addr = _8MB + (parent_pid * _4MB);
	virt_addr = _128MB;
	set_4MB(phys_addr, virt_addr, 3);
	
	/* Set up parent kernel stack addr for TSS */
	tss.esp0 = (uint32_t)(_8MB - (parent_pid * _8KB)-8);
	
	/* Jump to return from execute */
	halt_ret_exec(current_PCB->halt_back_ESP, status);

	/* Dummy return */
	return -1;
}

/*
 * system_open:
 * Description: Open system call kernel function
 * Input: File name of the file to return
 * Output: -1 on fail, file descriptor on success
 * Effect: Open the file
 */
int32_t system_open(const int8_t* fname){
	dentry_t dentry;
	file_desc_t file;

	/* Get dentry of the file */
	read_dentry_by_name(fname, &dentry);
	
	/* Assign correct file operations */
	if(dentry.file_type == 0)
		file.f_op = rtc_op;
	else if(dentry.file_type == 1)
		file.f_op = dir_op;
	else if(dentry.file_type == 2)
		file.f_op = data_op;
	else
		return -1;

	/* Call corresponding open serivice call */
	return file.f_op->open(fname);
}

/*
 * system_read:
 * Description: Read system call kernel function
 * Input: File descirptor of the file to read, a pointer to a buffer to read into 
 * 	  and the number of bytes to read
 * Output: -1 on fail, number of bytes actually read on success
 * Effect: Read the file
 */
int32_t system_read(int32_t fd, void* buf, uint32_t size){
	/* Call corresponding read serivice call */
	return current_PCB->file_desc_arr[fd].f_op->read(fd, buf, size);
}

/*
 * system_write:
 * Description: Write system call kernel function
 * Input: File descirptor of the file to write, a pointer to a buffer to write from
 * 	  and the number of bytes to write
 * Output: -1 on fail, number of bytes actually write on success
 * Effect: Write the file
 */
int32_t system_write(int32_t fd, const void* buf, uint32_t size){
	/* Call corresponding write serivice call */
	return current_PCB->file_desc_arr[fd].f_op->write(fd, buf, size);	
}

/*
 * system_close:
 * Description: Close system call kernel function
 * Input: File descriptor of the file to close
 * Output: -1 on fail, 0 on success
 * Effect: Close the file
 */
int32_t system_close(int32_t fd){
	/* Call corresponding close serivice call */
	return current_PCB->file_desc_arr[fd].f_op->close(fd);	
}







