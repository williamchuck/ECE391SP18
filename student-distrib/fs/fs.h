#ifndef FS_H_
#define FS_H_

#include "../types.h"

typedef struct dentry{
	uint8_t file_name[32];
	uint32_t file_type;
	uint32_t inode;	
} dentry_t;

typedef struct file_desc{
	struct file_op* f_op;
	uint32_t inode;
	uint32_t f_pos;
	uint8_t flag;
} file_desc_t;

typedef struct file_op{
	int32_t (*open)(const int8_t* fname);
	int32_t (*read)(int32_t fd, void* buf, int32_t size);
	int32_t (*write)(int32_t fd, const void* buf, int32_t size);
	int32_t (*close)(int32_t fd);	
} file_op_t;

extern int32_t read_dentry_by_name (const int8_t* fname, dentry_t* dentry);
extern int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
extern int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
extern void init_fs();

extern int32_t data_open(const int8_t* fname);
extern int32_t data_read(int32_t fd, void* buf, uint32_t size);
extern int32_t data_write(int32_t fd, const void* buf, int32_t size);
extern int32_t data_close(int32_t fd);

extern int32_t dir_open(const int8_t* fname);
extern int32_t dir_read(int32_t fd, void* buf, int32_t size);
extern int32_t dir_write(int32_t fd, const void* buf, int32_t size);
extern int32_t dir_close(int32_t fd);
#endif
