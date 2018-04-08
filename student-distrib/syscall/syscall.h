#ifndef SYSCALL_H_
#define SYSCALL_H_

#include "../types.h"

#define shell_PCB 0x7FE000
#define ls_PCB 0x7FC000
#define _128MB 0x80000000
#define _8MB 0x800000
#define _4MB 0x400000
#define _8KB 

extern int p_num;

extern int32_t system_execute(const int8_t* file_name);

extern int32_t system_halt(uint8_t status);

extern int32_t system_open(const int8_t* fname);

extern int32_t system_close(int32_t fd);

extern int32_t system_read(int32_t fd, void* buf, uint32_t size);

extern int32_t system_write(int32_t fd, const void* buf, uint32_t size);

extern void jump_to_user(void* entry_point);

#endif
