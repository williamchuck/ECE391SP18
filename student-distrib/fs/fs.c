#include "fs.h"
#include "../lib.h"

unsigned int dentry_count;
unsigned int inode_count;
unsigned int data_block_count;
unsigned char* fs_addr =(unsigned char*) 0x00410000;
unsigned int boot_block[1024];

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

void init_fs(){
	int i, index;
	dentry_count = *fs_addr;
	for(i = 0; i < 3; i++){
		dentry_count = dentry_count<<8;
		fs_addr++;
		dentry_count += *fs_addr;
	}
	fs_addr++;
	inode_count = *fs_addr;
	for(i = 0; i < 3;i++){
		inode_count = inode_count<<8;
		fs_addr++;
		inode_count += *fs_addr;	
	}
	fs_addr++;
	data_block_count = *fs_addr;
	for(i = 0; i < 3; i++){
		data_block_count = data_block_count<<8;
		fs_addr++;
		data_block_count += *fs_addr;
	}
	fs_addr = (unsigned char*)0x00410000;

	boot_block[0] = dentry_count;
	boot_block[1] = inode_count;
	boot_block[2] = data_block_count;

	index = 16;
	//printf("dentry count:%x, inode_count:%x, data_block_count:%x\n", dentry_count, inode_count, data_block_count);
}
