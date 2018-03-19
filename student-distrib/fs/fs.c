#include "fs.h"
#include "../lib.h"

static uint32_t dentry_count;
static uint32_t inode_count;
static uint32_t data_block_count;
static uint32_t* fs_addr =(uint32_t*) 0x00410000;

/*
 * init_fs:
 * Description: Initialize File system static variables for future use
 * Input: None
 * Output: None
 * Effects: Fill in three file system static variables for future use
 */
void init_fs(){
	/* Initialize addr for reading data */
	uint32_t* addr;

	/* Read dentry count */
	addr = fs_addr;
	dentry_count = *addr;
	
	/* Read inode count */
	addr++;
	inode_count = *addr;
	
	/* Read data block count */
	addr++;
	data_block_count = *addr;
	
	//printf("dentry count:0x%x, inode_count:0x%x, data_block_count:0x%x\n", dentry_count, inode_count, data_block_count);
}

int32_t read_dentry_by_name(const int8_t* fname, dentry_t* dentry){
	uint8_t* addr;
	uint8_t str[32];
	int i,j;

	addr = (uint8_t*)fs_addr + 64;

	for(i = 0; i < dentry_count; i++){
		printf(" ");

		const int8_t* name = fname;
		uint8_t same = 1;
		for(j = 0; j < 32; j++){
			str[j] = *addr;
//			printf("%c", str[j]);
//			printf("%c %d", *name, j);
			if(str[j] != *name && *name != '\0')
				same = 0;
			if(*name == '\0')
				name--;

			addr++;
			name++;
		}
		
		if(same){
//			printf("SAME");
			for(j = 0; j < 32; j++){
//				printf("%c", str[j]);
				dentry->file_name[j] = str[j];
			}
//			printf("\n");
			dentry->file_type = *((uint32_t*)addr);
			addr += 4;
			dentry->inode = *((uint32_t*)addr);
			return 0;
		}
		addr += 32;
		printf("\n");
	}

	return -1;
}

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
	if(index > (dentry_count - 1))
		return -1;

	uint8_t* addr;
	int i;

	addr = (uint8_t*)fs_addr + 64;

	addr += index * 64;

	for(i = 0; i < 32; i++){
		dentry->file_name[i] = *addr;
		addr++;
	}
	dentry->file_type = *((uint32_t*)addr);
	addr += 4;
	dentry->inode = *((uint32_t*)addr);
	return 0;
}
