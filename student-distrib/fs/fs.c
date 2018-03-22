#include "fs.h"
#include "../lib.h"

static uint32_t dentry_count;
static uint32_t inode_count;
static uint32_t data_block_count;
static uint32_t* fs_addr;
static file_desc_t file_desc[8];

static file_op_t data_file_op = {
	.open = data_open,
	.read = data_read,
	.write = data_write,
	.close = data_close
};

static file_op_t dir_file_op = {
	.open = dir_open,
	.read = dir_read,
	.write = dir_write,
	.close = dir_close
};

/*
 * init_fs:
 * Description: Initialize File system static variables for future use
 * Input: None
 * Output: None
 * Effects: Fill in three file system static variables for future use
 */
void init_fs(uint32_t* file_system_addr){
	/* Initialize addr for reading data */
	uint32_t* addr;

	fs_addr = file_system_addr;

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
		if(((offset % 4096) + i) > (byte_left - 1))
			return i;

		buf[i] = *(data_addr + i);
	}

	return length;
}

int32_t data_open(const int8_t* fname){
	int i;
	dentry_t dentry;

	for(i = 0; i < 32; i++)
		dentry.file_name[i] = 0;

	if(read_dentry_by_name(fname, &dentry) == -1)
		return -1;

	//printf("inode: %d\n", dentry.inode);
	
//	printf("Open ");
//	for(i = 0; i < 32; i++){
//		if(dentry.file_name[i] == 0)
//			break;
//		printf("%c", dentry.file_name[i]);
//	}
//	printf(", Type: %d, inode: %d\n", dentry.file_type, dentry.inode);

	for(i = 2; i < 8; i++){
		if(file_desc[i].flag == 0){
			file_desc[i].f_op = &data_file_op;
			file_desc[i].inode = dentry.inode;
			file_desc[i].f_pos = 0;
			file_desc[i].flag = 1;
			return i;
		}
	}
	
	return -1;
}

int32_t data_read(int32_t fd, void* buf, uint32_t size){
	if(fd > 7 || fd < 0)
		return -1;

	int ret;
	ret = read_data(file_desc[fd].inode, file_desc[fd].f_pos, (uint8_t*)buf, size);
	file_desc[fd].f_pos += ret;
	return ret;
}

int32_t data_write(int32_t fd, const void* buf, uint32_t size){
	return -1;
}

int32_t data_close(int32_t fd){
	if(fd < 0 || fd > 7)
		return -1;

	if(file_desc[fd].flag == 0)
		return -1;
	
	file_desc[fd].f_op = NULL;
	file_desc[fd].inode = 0;
	file_desc[fd].f_pos = 0;
	file_desc[fd].flag = 0;

	return 0;
}

int32_t dir_open(const int8_t* fname){
	int i;
	dentry_t dentry;

	read_dentry_by_name(fname, &dentry);

	if(dentry.file_type != 1)
		return -1;

	for(i = 2; i < 8; i++){
		if(file_desc[i].flag == 0){
			file_desc[i].f_op = &dir_file_op;
			file_desc[i].inode = 0;
			file_desc[i].f_pos = 0;
			file_desc[i].flag = 1;
			return i;
		}
	}

	return -1;
}

int32_t dir_read(int32_t fd, void* buf, uint32_t size){
	return 0;
}

int32_t dir_write(int32_t fd, const void* buf, uint32_t size){
	return -1;
}

int32_t dir_close(int32_t fd){
	if(fd < 0 || fd > 7)
		return -1;

	if(file_desc[fd].flag == 0)
		return -1;
	
	file_desc[fd].f_op = NULL;
	file_desc[fd].inode = 0;
	file_desc[fd].f_pos = 0;
	file_desc[fd].flag = 0;

	return 0;	
}













