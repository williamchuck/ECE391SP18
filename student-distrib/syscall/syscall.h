#ifndef SYSCALL_H_
#define SYSCALL_H_

#include "../types.h"
#include "../fs/fs.h"
#include "../page/page.h"
#include "../lib.h"

#define shell_PCB 0x7FE000
#define ls_PCB 0x7FC000
#define 128MB 0x80000000
#define 8MB 0x800000
#define 4MB 0x400000

extern void syscall_asm_linkage();

extern int32_t system_execute(const int8_t* file_name);

extern int32_t system_halt(uint8_t status);

extern int32_t system_open(const int8_t* fname);

extern int32_t system_close(int32_t fd);

extern int32_t system_read(int32_t fd, void* buf, int32_t size);

extern int32_t system_write(int32_t fd, const void* buf, int32_t size);

extern int p_num;
#endif
