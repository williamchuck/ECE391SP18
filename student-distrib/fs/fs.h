#ifndef FS_H_
#define FS_H_

#include "../types.h"

typedef struct dentry{
	uint8_t file_name[32];
	uint32_t file_type;
	uint32_t inode;	
} dentry_t;

extern void init_fs();

#endif
