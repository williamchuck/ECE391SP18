#include "fs.h"
#include "../lib.h"

static uint32_t dentry_count;
static uint32_t inode_count;
static uint32_t data_block_count;
static uint32_t* fs_addr =(uint32_t*) 0x00410000;
static file_desc_t file_desc[8];

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

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
	if(inode > (inode_count - 1))
		return -1;

	uint32_t file_size, data_block_number, byte_left;
	uint32_t* inode_addr, db_addr;
	uint8_t* data_addr;
	int i;

	inode_addr = fs_addr + 1024 + 1024 * inode;
	db_addr = fs_addr + 1024 + 1024 * inode_count;
	file_size = *inode_addr;
	if(file_size == 0)
		return -1;

	inode_addr++;
	uint32_t* temp = inode_addr;
//	printf("Size: %d\n", file_size);

	for(i = 0; i <= (file_size/4096); i++){
		//printf("data_block #: %d\n", *temp);
		if(*temp > (data_block_count - 1))
			return -1;
		temp++;
	}

	
	data_block_number = *(inode_addr + (offset/4096));
	byte_left = file_size - 4096 * (offset/4096);
	//printf("ByteLeft: %d\n", byte_left);

	data_addr = (uint8_t*)((uint8_t*)db_addr + 4096 * data_block_number + (offset % 4096));

	for(i = 0; i < length; i++){
		if(((offset % 4096) + i) >= (byte_left - 1))
			return i;

		buf[i] = *(data_addr + i);
	}

	return length;
}

int32_t f_open(const int8_t* fname){
	int i;
	dentry_t dentry;

	for(i = 0; i < 32; i++)
		dentry.file_name[i] = 0;

	if(read_dentry_by_name(fname, &dentry) == -1)
		return -1;

	if(dentry.file_type == 0){
		printf("Open rtc, Type: %d, inode: N/A\n", dentry.file_type);
	}
	else if(dentry.file_type == 1){
		printf("Open ");
		for(i = 0; i < 32; i++){
			if(dentry.file_name[i] == 0)
				break;
			printf("%c", dentry.file_name[i]);
		}
		printf(", Type: %d, inode: N/A\n", dentry.file_type);
	}
	else if(dentry.file_type == 2){
		printf("Open ");
		for(i = 0; i < 32; i++){
			if(dentry.file_name[i] == 0)
				break;
			printf("%c", dentry.file_name[i]);
		}
		printf(", Type: %d, inode: %d\n", dentry.file_type, dentry.inode);
	}

	return 0;
}





















