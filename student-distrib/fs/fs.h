#ifndef FS_H_
#define FS_H_

#include "../types.h"

typedef struct dentry{
	uint8_t file_name[32];
	uint32_t file_type;
	uint32_t inode;	
} dentry_t;


extern int32_t read_dentry_by_name (const int8_t* fname, dentry_t* dentry);
extern int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
extern int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
extern void init_fs();

#endif
