#ifndef SYSCALL_H_
#define SYSCALL_H_

#include "../types.h"

//#define shell_PCB 0x7FE000
//#define ls_PCB 0x7FC000

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

extern int32_t system_open(const int8_t* fname);

extern int32_t system_close(int32_t fd);

extern int32_t system_read(int32_t fd, void* buf, uint32_t size);

extern int32_t system_write(int32_t fd, const void* buf, uint32_t size);

extern int32_t system_getargs(uint8_t* buf, int32_t nbytes);

extern int32_t system_vidmap(uint8_t** screen_start);

/* Return to user space */
extern int32_t jump_to_user(void* entry_point, void* ESP, void** haltESP_loc);
/* Return from execute */
extern void halt_ret_exec(void* back_ESP, uint32_t status);

/* Buffer for shell command. Copied from terminal buffer upon execution */
uint8_t shell_buf[BUF_SIZE];
#endif
