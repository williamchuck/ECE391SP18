#include "fs.h"
#include "../lib.h"

uint32_t dentry_count;
uint32_t inode_count;
uint32_t data_block_count;
uint32_t* fs_addr =(uint32_t*) 0x00410000;


void init_fs(){
	int i;

	dentry_count = *fs_addr;
	/*
	for(i = 0; i < 3; i++){
		dentry_count = dentry_count<<8;
		fs_addr++;
		dentry_count += *fs_addr;
	}*/
	fs_addr++;
	inode_count = *fs_addr;
	/*
	for(i = 0; i < 3;i++){
		inode_count = inode_count<<8;
		fs_addr++;
		inode_count += *fs_addr;	
	}*/
	fs_addr++;
	data_block_count = *fs_addr;
	/*
	for(i = 0; i < 3; i++){
		data_block_count = data_block_count<<8;
		fs_addr++;
		data_block_count += *fs_addr;
	}*/
	fs_addr = (uint32_t*)0x00410000;
	printf("dentry count:0x%x, inode_count:0x%x, data_block_count:0x%x\n", dentry_count, inode_count, data_block_count);
}

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
	uint8_t* addr;
	uint8_t str[32];
	int i,j;

	addr = (uint8_t*)fs_addr + 64;

	for(i = 0; i < dentry_count; i++){
	//	printf(" ");
		for(j = 0; j < 32; j++){
			str[j] = *addr;
	//		printf("%c", str[j]);
			addr++;
		}
		addr += 32;
	//	printf("\n");
	}

	return 0;
}
