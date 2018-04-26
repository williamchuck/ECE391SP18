#ifndef SYSCALL_H_
#define SYSCALL_H_

#include "../types.h"
#include "../regs.h"

/* Define different sizes for reference */
#define _128MB 0x8000000
#define _8MB 0x800000
#define _4MB 0x400000
#define _8KB 0x2000
#define OFFSET 0x48000
#define EXE_MAGIC 0x464C457F
#define ENTRY_POS_OFFSET 24

#define FD_MIN 0
#define FD_MAX 7

#define BUF_SIZE 128

/* Kernel functions for system calls */
extern int32_t system_execute(const int8_t* file_name);

extern int32_t system_halt(uint8_t status);

extern int32_t system_internal_halt(uint32_t status);

extern int32_t system_open(const int8_t* fname);

extern int32_t system_close(int32_t fd);

extern int32_t system_read(int32_t fd, void* buf, uint32_t size);

extern int32_t system_write(int32_t fd, const void* buf, uint32_t size);

extern int32_t system_getargs(uint8_t* buf, int32_t nbytes);

extern int32_t system_vidmap(uint8_t** screen_start);

/* Return to user space */
extern int32_t jump_to_user(void* entry_point, void* ESP);
/* C dispatcher for system calls */
extern void do_sys(regs_t* regs);

/* Buffer for shell command. Copied from terminal buffer upon execution */
uint8_t shell_buf[BUF_SIZE];
#endif
