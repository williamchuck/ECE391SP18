/*
 * syscall_user.h
 *
 * userspace function for syscalling
 *
 *  Created on: Apr 7, 2018
 *      Author: Dennis
 */

#ifndef SYSCALL_USER_H_
#define SYSCALL_USER_H_
#include "../types.h"

/* User function for system calls */
extern int32_t ece391_execute(const int8_t* file_name);

extern int32_t ece391_halt(uint8_t status);

extern int32_t ece391_open(const int8_t* fname);

extern int32_t ece391_close(int32_t fd);

extern int32_t ece391_read(int32_t fd, void* buf, uint32_t size);

extern int32_t ece391_write(int32_t fd, const void* buf, uint32_t size);



#endif /* SYSCALL_USER_H_ */
