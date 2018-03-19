#include "fs.h"
#include "../lib.h"

uint32_t dentry_count;
uint32_t inode_count;
uint32_t data_block_count;
uint32_t* fs_addr =(uint32_t*) 0x00410000;


void init_fs(){
	uint32_t* addr;

	addr = fs_addr;
	dentry_count = *addr;
	
	addr++;
	inode_count = *addr;
	
	addr++;
	data_block_count = *addr;
	
	//printf("dentry count:0x%x, inode_count:0x%x, data_block_count:0x%x\n", dentry_count, inode_count, data_block_count);
}

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
	uint8_t* addr;
	uint8_t str[32];
	int i,j;

	addr = (uint8_t*)fs_addr + 64;

	for(i = 0; i < dentry_count; i++){
		printf(" ");

		const uint8_t* name = fname;
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
