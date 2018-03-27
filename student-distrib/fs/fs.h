#ifndef FS_H_
#define FS_H_

#include "../types.h"

typedef struct dentry{
	/* 32 file name upper bound plus a null terminate */
	int8_t file_name[33];
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
	int32_t (*read)(int32_t fd, void* buf, uint32_t size);
	int32_t (*write)(int32_t fd, const void* buf, uint32_t size);
	int32_t (*close)(int32_t fd);	
} file_op_t;

/* Fill a dentry given file name */
extern int32_t read_dentry_by_name (const int8_t* fname, dentry_t* dentry);
/* Fill a dentry given a index into boot block */
extern int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
/* Read data into a buf given inode and position in file */
extern int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
/* Initialize file system */
extern void init_fs(uint32_t* file_system_addr);
/* Get size of a file given file name */
extern uint32_t get_size(const int8_t* fname);

/* Open a data file*/
extern int32_t data_open(const int8_t* fname);
/* Read content of a data file into a buf */
extern int32_t data_read(int32_t fd, void* buf, uint32_t size);
/* Write content of buf into a data file (Disabled) */
extern int32_t data_write(int32_t fd, const void* buf, uint32_t size);
/* Close a data_file */
extern int32_t data_close(int32_t fd);

/* Open a directory */
extern int32_t dir_open(const int8_t* fname);
/* Read file names in the directory into buf */
extern int32_t dir_read(int32_t fd, void* buf, uint32_t size);
/* Write to the directory (Disabled) */
extern int32_t dir_write(int32_t fd, const void* buf, uint32_t size);
/* Close a directory */
extern int32_t dir_close(int32_t fd);
#endif
