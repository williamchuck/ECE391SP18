#include "syscall.h"
#include "../fs/fs.h"
#include "../page/page.h"
#include "../lib.h"
#include "../x86_desc.h"
#include "../process/process.h"
#include "../drivers/stdin.h"
#include "../drivers/stdout.h"

/*
 * get_free_pid:
 * Description: Get a free pid for child process
 * Input: None
 * Output: pid for child process
 * Effect: None
 */
int32_t get_free_pid(){
    int i;
    for(i = 0; i < NUM_PROC; i++){
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
    int fd, size, child_pid, i, j;
    void* entry_addr;
    uint32_t child_kernel_ESP, user_ESP, phys_addr, virt_addr;
    int8_t command[BUF_SIZE];
    int8_t cmd_started;

    /* Get pid for child */
    child_pid = get_free_pid();
    /* Check if pid is valid */
    if(child_pid == -1)
        return -1;
    //check if provided virtual memory page is present
    if(!page_present(file_name))return -1;

    /* Copy filename to shell buffer */
    /* Also clears command var. */
    for(i = 0; i < BUF_SIZE; i++){
        shell_buf[i] = file_name[i];
        command[i] = 0x00;
    }

    /* Extract actual command from entire shell line. */
    cmd_started = 0;
    j = 0;

    /* Read command. (First word starting with non-space/null and terminated by space/null */
    for(i = 0; i < BUF_SIZE; i++){
        /* If reaching NULL, break. */
        if(file_name[i] == 0x00)
            break;
        /* When reach non-space chars, copy command. */
        if(file_name[i] != ASCII_SPACE){
            if(cmd_started == 0)
                cmd_started = 1;
            command[j] = file_name[i];
            j++;
        }
        /* When encounter space separating command and arg, break. */
        else if((file_name[i] == ASCII_SPACE) && (cmd_started == 1))
            break;
    }

    /* open file */
    fd = system_open(command);
    if(fd == -1)
        return -1;
    //check that it's a data file
    if(current_PCB->file_desc_arr[fd].f_op != data_op){
        system_close(fd);
        return -1;
    }
    /* Get file size before switching page since the string wouldn't be accessible once page is switched */
    size = get_size(command);
    if(size<ENTRY_POS_OFFSET){//if the file didn't reach the size where entry location is recorded, then it's not executable
        system_close(fd);
        return -1;
    }

    /* Set up correct memory mapping for child process */
    phys_addr = _8MB + (child_pid * _4MB);
    virt_addr = _128MB;
    set_4MB(phys_addr, virt_addr, 3);

    /* Load user program */
    uint8_t* buf=(uint8_t*)(virt_addr + OFFSET);
    system_read(fd, buf, size);
    //close file as we have finished loading the contents
    system_close(fd);

    /* Check if file is executable */
    if( *((uint32_t*)buf) != EXE_MAGIC){
        /* Remap memory back to current process */
        set_4MB(_8MB + (current_PCB->pid * _4MB), virt_addr, 3);
        return -1;
    }

    /* Mark child pid as used */
    process_desc_arr[child_pid].flag = 1;

    /* Get the entry addr:
     * get address of buf[ENTRY_POS_OFFSET],
     * reinterpret that as a double pointer,
     * dereference it to get entry addr */
    entry_addr = *((void**)(&(buf[ENTRY_POS_OFFSET])));

    /* Set up user esp at the bottom of the user page */
    user_ESP = virt_addr + _4MB;
    /* Set up child process kernel esp at the bottom of the 8KB block */
    child_kernel_ESP = _8MB - (child_pid * _8KB);

    /* Set TSS esp0 to child process kernel stack */
    tss.esp0 = child_kernel_ESP;

    /* Calculate location of child PCB
     * top of the 8KB child kernel stack region is for PCB
     * as outlined in get_current_PCB */
    PCB_block_t* child_PCB;
    child_PCB = (PCB_block_t*)((child_kernel_ESP-1) & _8KB_MASK);

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
    system_internal_halt(status);
    return -1;
}
/*
 * system_halt:
 * Description: Halt system call kernel function
 * Input: Return value to user space
 * Output: Always -1 because it should not return
 * Effect: Halt the file
 */
int32_t system_internal_halt(uint32_t status){
    uint32_t pid, phys_addr, virt_addr, parent_pid;
    pid = current_PCB->pid;

    /* Unset flag for current pid */
    process_desc_arr[pid].flag = 0;

    /* Get parent pid */
    parent_pid = current_PCB->parent_PCB->pid;

    if(parent_pid!=0){//if parent process is a user program
        /* Get parent's userspace mem addr to remap */
        phys_addr = _8MB + (parent_pid * _4MB);
        virt_addr = _128MB;
        set_4MB(phys_addr, virt_addr, 3);
    }
    else{//if no more user program running
        free_4MB(_128MB);//free the 4MB page at 128MB
    }

    free_4KB(_128MB+_4MB);//free the vidmap for user program

    /* Set up parent kernel stack addr for TSS */
    tss.esp0 = (uint32_t)(_8MB - (parent_pid * _8KB));

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
    //check if provided virtual memory page is present
    if(!page_present(fname))return -1;
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
 *    and the number of bytes to read
 * Output: -1 on fail, number of bytes actually read on success
 * Effect: Read the file
 */
int32_t system_read(int32_t fd, void* buf, uint32_t size){
    //check if provided virtual memory page is present
    if(!page_present(buf))return -1;
    //fd range check
    if(fd<FD_MIN||fd>FD_MAX)return -1;
    //check if fd loaded
    if(!current_PCB->file_desc_arr[fd].flag)return -1;
    /* Call corresponding read serivice call */
    return current_PCB->file_desc_arr[fd].f_op->read(fd, buf, size);
}

/*
 * system_write:
 * Description: Write system call kernel function
 * Input: File descirptor of the file to write, a pointer to a buffer to write from
 *    and the number of bytes to write
 * Output: -1 on fail, number of bytes actually write on success
 * Effect: Write the file
 */
int32_t system_write(int32_t fd, const void* buf, uint32_t size){
    //check if provided virtual memory page is present
    if(!page_present(buf))return -1;
    //fd range check
    if(fd<FD_MIN||fd>FD_MAX)return -1;
    //check if fd loaded
    if(!current_PCB->file_desc_arr[fd].flag)return -1;
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
    //fd range check
    if(fd<FD_MIN||fd>FD_MAX)return -1;
    //check if fd loaded
    if(!current_PCB->file_desc_arr[fd].flag)return -1;
    //stdin and out can't be closed
    if(fd==0||fd==1)return -1;
    /* Call corresponding close serivice call */
    return current_PCB->file_desc_arr[fd].f_op->close(fd);
}

/*
 * system_getargs
 * DESCRIPTION: get arguments in a comman line
 * INPUTS: buf - target buffer to be copied to.
 *               nbytes - number of bytes in target buffer
 * OUTPUT: none.
 * RETURN VALUE: -1 when failed. number of bytes copied when success.
 * SIDE EFFECTS: none.
 */
int32_t system_getargs(uint8_t* buf, int32_t nbytes)
{
    //check if provided virtual memory page is present
    if(!page_present(buf))return -1;

    /* Starting and Finising index of argument in buffer. */
    int arg_startindex;
    int arg_finishindex;

    /* Loop var.*/
    int i;

    /* Flags for traversing through buffer. */
    uint8_t cmd_started;
    uint8_t cmd_finished;
    uint8_t arg_started;
    uint8_t arg_finished;

    /* Reset arguments and indexes. */
    cmd_started = 0;
    cmd_finished = 0;
    arg_started = 0;
    arg_finished = 0;
    arg_startindex = -1;
    arg_finishindex = -1;

    /* Traverse through entire buffer. Shell lines are always NULL terminated. */
    for (i = 0; i < BUF_SIZE; i++)
    {
        /* When reach newline, finish traversing. */
        if (shell_buf[i] == 0x00)
        {
            arg_finished = 1;
            arg_finishindex = i;
            break;
        }
        /* If command has been read, set flag. */
        if ((cmd_started == 0) && shell_buf[i] != ASCII_SPACE)
        {
            cmd_started = 1;
            continue;
        }
        /* If command has ended (space), set flag. */
        if ((cmd_started == 1) && (cmd_finished == 0) && (shell_buf[i] == ASCII_SPACE))
        {
            cmd_finished = 1;
            continue;
        }
        /* If argument has started, set index and flag. */
        if ((cmd_finished == 1) && (arg_started == 0) && (shell_buf[i] != ASCII_SPACE))
        {
            arg_started = 1;
            arg_startindex = i;
            continue;
        }
    }

    /* If no argument or invalid argument, return error. */
    if ((arg_startindex == -1) || arg_finishindex < arg_startindex)
    {
        return -1;
    }

    /* If argument too large for buffer, return -1. */
    if (arg_finishindex - arg_startindex + 1 > nbytes)
    {
        return -1;
    }

    /* Clear target buffer with NULL char. */
    for (i = 0; i < nbytes; i++)
    {
        buf[i] = 0x00;
    }

    /* Copy argument into target buffer. */
    for (i = arg_startindex; i <= arg_finishindex; i++)
    {
        buf[i - arg_startindex] = shell_buf[i];
    }

    /* Return success. */
    return 0;
}

/*
 * system_vidmap
 * DESCRIPTION: maps the text-mode video memory into user space at a pre-set virtual address.
 * INPUTS: screen_start: virtual address of video memory.
 * OUTPUT: none.
 * RETURN VALUE: -1 when failed. 0 when success.
 * SIDE EFFECTS: Text-mode video memory is mapped into user space.
 */
int32_t system_vidmap(uint8_t** screen_start)
{
    //check if provided virtual memory page is present
    if(!page_present(screen_start))return -1;

    /* Map vmem into user space address. */
    set_4KB(VIDEO_MEM, _128MB + _4MB, 3);
    *screen_start = (uint8_t*)(_128MB + _4MB);
    return 0;
}
