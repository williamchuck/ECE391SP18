#include "fs.h"
#include "../lib.h"
#include "../tests.h"

#define FILE_NAME_LENGTH 32
#define BLOCK_SIZE 4096
#define BYTES_PER_INT 4
#define BOOT_BLOCK_ENTRY_SIZE 64
#define INT_PER_BLOCK BLOCK_SIZE/BYTES_PER_INT

/* Count for dentry, inode and data block */
static uint32_t dentry_count;
static uint32_t inode_count;
static uint32_t data_block_count;
/* Start address of the file system */
static uint32_t* fs_addr;

/* Two jump tables for future usage */
//static file_op_t data_file_op = {
//	.open = data_open,
//	.read = data_read,
//	.write = data_write,
//	.close = data_close
//};

//static file_op_t dir_file_op = {
//	.open = dir_open,
//	.read = dir_read,
//	.write = dir_write,
//	.close = dir_close
//};

/*
 * init_fs:
 * Description: Initialize File system static variables for future use
 * Input: Start of the file system
 * Output: None
 * Effects: Fill in three file system static variables for future use
 */
void init_fs(uint32_t* file_system_addr){
	/* Initialize addr for reading data */
	uint32_t* addr;

	/* Initialize start of the file system */
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
}

/*
 * get_size:
 * Description: Return the size of the file given file name
 * Input: File name
 * Output: File size
 * Effect: None
 */
uint32_t get_size(const int8_t* fname){
	/* Initialize variables */
	uint32_t* addr;
	dentry_t dentry;

	/* Fill in dentry */
	if(read_dentry_by_name(fname, &dentry) == -1)
		return -1;
	
	/* Read size according to inode */
	addr = fs_addr + INT_PER_BLOCK * (dentry.inode + 1);

	return *addr;
}

/*
 * read_dentry_by_name:
 * Description: Fill in a dentry_t given the file name
 * Input: File name and the addr of the dentry to fill in
 * Output: -1 on fail, 0 on success
 * Effect: Fill in the dentry with info
 */
int32_t read_dentry_by_name(const int8_t* fname, dentry_t* dentry){
	/* Initialize variables */
	uint8_t* addr;
	uint8_t str[FILE_NAME_LENGTH];
	int i, j, size;

	/* Check for size of input fname */
	size = 0;
	while(*(fname + size) != '\0')
		size++;

	/* If it is too long, return -1 */
	if(size > FILE_NAME_LENGTH)
		return -1;

	/* Point addr to the start of dentries */
	addr = (uint8_t*)fs_addr + BOOT_BLOCK_ENTRY_SIZE;

	/* Search for dentry with file name */
	for(i = 0; i < dentry_count; i++){
		/* Temporary pointer to file name */
		const int8_t* name = fname;
		/* Flag to check for match */
		uint8_t same = 1;
		/* Check if file name is the same */
		for(j = 0; j < FILE_NAME_LENGTH; j++){
			/* Fill in str for future use */
			str[j] = *addr;
			/* If dismatch or no more to read, clear same flag */
			if(str[j] != *name && *name != '\0')
				same = 0;
			/* If no more to read, wait on the same place */
			if(*name == '\0')
				name--;

			/* Progress one step */
			addr++;
			name++;
		}
		
		/* If same, then fill in the dentry and return 0 */
		if(same){
			/* Fill in dentry file_name */
			for(j = 0; j < FILE_NAME_LENGTH; j++)
				dentry->file_name[j] = str[j];

			dentry->file_name[FILE_NAME_LENGTH] = '\0';
			/* Fill in dentry file_type */
			dentry->file_type = *((uint32_t*)addr);
			/* Point addr to inode */
			addr += BYTES_PER_INT;
			/* Fill in dentry inode */
			dentry->inode = *((uint32_t*)addr);
			/* Return 0 on success */
			return 0;
		}
		/* If dismatch, move on to next dentry */
		addr += 32;
	}

	/* If file does not exist, return -1 */
	return -1;
}

/*
 * read_dentry_by_index:
 * Description: Fill the dentry_t with info given the index
 * Input: Index of the dentry and the address of the dentry to fill in
 * Ouput: -1 on fail, 0 on success
 * Effect: Fill in dentry with the info
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
	/* Sanity check */
	if(index > (dentry_count - 1))
		return -1;

	/* Initialize variables */
	uint8_t* addr;
	int i;

	/* Point addr to the start of dentries */
	addr = (uint8_t*)fs_addr + BOOT_BLOCK_ENTRY_SIZE;

	/* Point addr to the dentry we want */
	addr += index * BOOT_BLOCK_ENTRY_SIZE;

	/* Fill in dentry_t file_name */
	for(i = 0; i < FILE_NAME_LENGTH; i++){
		dentry->file_name[i] = *addr;
		addr++;
	}
	dentry->file_name[FILE_NAME_LENGTH] = '\0';
	/* Fill in dentry_t file_type */
	dentry->file_type = *((uint32_t*)addr);
	/* Point addr to inode */
	addr += BYTES_PER_INT;
	/* Fill in dentry_t inode */
	dentry->inode = *((uint32_t*)addr);
	/* Return 0 on success */
	return 0;
}

/*
 * read_date:
 * Description: Read data to a buffer with given length
 * Input: inode number and position in the file, pointer to the buffer to fill in and the number of bytes to copy
 * Output: Number of bytes actually copied
 * Effect: Copy data from file into the buffer
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
	/* Sanity check */
	if(inode > (inode_count - 1))
		return -1;

	/* Initialize variables */
	uint32_t file_size, data_block_number, byte_left;
	uint32_t* inode_addr;
	uint32_t* db_addr;
	uint8_t* data_addr;
	int i, j;

	/* Point inode_addr to start of inode */
	inode_addr = fs_addr + INT_PER_BLOCK * (inode + 1);
	/* Point db_addr to start of data block entries */
	db_addr = fs_addr + INT_PER_BLOCK * (inode_count + 1);
	/* Read in file_size */
	file_size = *inode_addr;
	/* If this inode is not used, return -1 */
	if(file_size == 0)
		return -1;

	/* Point inode_addr to first data block number */
	inode_addr++;
	/* Read in first data block number */
	uint32_t* temp = inode_addr;

	/* Sanity check for every data block number */
	for(i = 0; i <= (file_size/BLOCK_SIZE); i++){
		if(*temp > (data_block_count - 1))
			return -1;
		temp++;
	}

	/* Set data_block_number */	
	data_block_number = *(inode_addr + (offset/BLOCK_SIZE));
	/* Set bytes left to read in current data block */
	byte_left = file_size - BLOCK_SIZE * (offset/BLOCK_SIZE);
	/* Point data_addr to where we start to read */
	data_addr = (uint8_t*)((uint8_t*)db_addr + BLOCK_SIZE * data_block_number + (offset % BLOCK_SIZE));

	/* Read in data to buffer */
	for(i = 0, j = 0; i < length; i++, j++){
		/* If no more byte to read, return number of bytes read */
		if(((offset % BLOCK_SIZE) + j) > (byte_left - 1))
			return i;

		/* If need to switch data block */
		if(((offset % BLOCK_SIZE) + j) > (BLOCK_SIZE - 1)){
			/* Update offset */
			offset = offset + i;
			/* Reset position in data block to 0 */
			j = 0;
			/* Reset data_block_number */	
			data_block_number = *(inode_addr + (offset/BLOCK_SIZE));
			/* Reset bytes left to read in current data block */
			byte_left = file_size - BLOCK_SIZE * (offset/BLOCK_SIZE);
			/* Point data_addr to where we start to read */
			data_addr = (uint8_t*)((uint8_t*)db_addr + BLOCK_SIZE * data_block_number + (offset % BLOCK_SIZE));
		}
		/* Copy value from file to buffer */
		buf[i] = *(data_addr + j);
	}

	/* If read in fully complete, return number of bytes read */
	return i;
}

/*
 * data_open:
 * Description: open up a data file
 * Input: File name
 * Output: 0 on success, -1 on fail
 * Effect: Fill in file descriptor array
 */
int32_t data_open(const int8_t* fname){
	/* Initialize variables */
	int i;
	dentry_t dentry;

	for(i = 0; i < FILE_NAME_LENGTH; i++)
		dentry.file_name[i] = 0;

	/* Fill in dentry and check if file exist */
	if(read_dentry_by_name(fname, &dentry) == -1)
		return -1;

	/* Sanity check */
	if(dentry.file_type != 2)
		return -1;

	/* Reserve 0 and 1 for stdin and stdout, search for a free entry */
	//for(i = 2; i < 8; i++){
		/* If free, then fill in file_desc_t */
	//	if(file_desc[i].flag == 0){
	//		file_desc[i].f_op = &data_file_op;
	//		file_desc[i].inode = dentry.inode;
	//		file_desc[i].f_pos = 0;
	//		file_desc[i].flag = 1;
			/* Change global fd */
	//		fd = i;
	//		return 0;
	//	}
	//}
	/* If file desc array is full, return -1 */
	return 0;
}

/*
 * data_read:
 * Description: Read in data from a data file
 * Input: File descriptor and a pointer to the buf to fill and size to read
 * Output: Number of bytes actually read, -1 on fail
 * Effect: Fill in the buf with the data from data file
 */
int32_t data_read(int32_t fd, void* buf, uint32_t size){
	/* Sanity check */
	if(fd > 7 || fd < 0)
		return -1;

	/* Initialize varaibles */
	int ret;
	/* Read data from the data file using file descriptor */
	ret = read_data(file_desc[fd].inode, file_desc[fd].f_pos, (uint8_t*)buf, size);
	/* Progress in the file */
	file_desc[fd].f_pos += ret;
	/* Return number of bytes actually read */
	return ret;
}

/*
 * data_write:
 * Description: None
 * Input: File descriptor, pointer of a buf and size to write
 * Output: Always return -1
 * Effect: None
 */
int32_t data_write(int32_t fd, const void* buf, uint32_t size){
	return -1;
}

/*
 * data_close:
 * Description: Close the data file
 * Input: File descriptor
 * Output: -1 on fail, 0 on success
 * Effect: Clean up file desc array entry
 */
int32_t data_close(int32_t fd){
	/* Sanity Check */
	if(fd < 0 || fd > 7)
		return -1;

	/* If file is already free, return -1 */
	if(file_desc[fd].flag == 0)
		return -1;
	/* Clean up file desc array entry */
	file_desc[fd].f_op = NULL;
	file_desc[fd].inode = 0;
	file_desc[fd].f_pos = 0;
	file_desc[fd].flag = 0;

	/* Return 0 on success */
	return 0;
}

/*
 * dir_open:
 * Description: open up a directory
 * Input: Directory name
 * Output: 0 on success, -1 on fail
 * Effect: Fill in file descriptor array
 */
int32_t dir_open(const int8_t* fname){
	/* Initialize varaibles */
	//int i;
	dentry_t dentry;

	/* Fill in dentry */
	if(read_dentry_by_name(fname, &dentry) == -1)
		return -1;

	/* Sanity check */
	if(dentry.file_type != 1)
		return -1;

	/* Reserve file descriptor 0 and 1 for stdin and stdout, search for free entry */
	//for(i = 2; i < 8; i++){
		/* If free, then fill in the file_desc_t */
	//	if(file_desc[i].flag == 0){
	///		file_desc[i].f_op = &dir_file_op;
	//		file_desc[i].inode = 0;
	//		file_desc[i].f_pos = 0;
	//		file_desc[i].flag = 1;
			/* Change global fd */
	//		fd = i;
	//		return 0;
	//	}
	//}

	/* If file_desc is full, return -1 */
	return 0;
}

/*
 * dir_read:
 * Description: Read filenames in a directory to buf
 * Input: File descriptor, pointer to the buffer and the number of bytes to read
 * Ouput: Number of bytes actually read, -1 on fail
 * Effect: Fill buf with the filename of current file
 */
int32_t dir_read(int32_t fd, void* buf, uint32_t size){
	/* Sanity check */
	if(fd < 0 || fd > 7)
		return -1;

	/* Initialize variables */
	int i;
	dentry_t dentry;

	/* Fill dentry and check if index is valid */
	if(read_dentry_by_index(file_desc[fd].f_pos, &dentry) == -1)
		return 0;

	/* Progress one file */
	file_desc[fd].f_pos++;

	/* Copy filename of current file into buf */
	for(i = 0; i < size; i++){
		/* No more than 32 bytes is allowed */
		if(i >= FILE_NAME_LENGTH)
			break;
		/* Copy filename to buf */
		((uint8_t*)buf)[i] = dentry.file_name[i];
	}

	/* Return number of bytes actually read */
	return i;
}

/*
 * dir_write:
 * Description: None
 * Input: File descriptor, pointer of a buf and size to write
 * Output: Always return -1
 * Effect: None
 */
int32_t dir_write(int32_t fd, const void* buf, uint32_t size){
	return -1;
}

/*
 * data_close:
 * Description: Close the directory
 * Input: File descriptor
 * Output: -1 on fail, 0 on success
 * Effect: Clean up file desc array entry
 */
int32_t dir_close(int32_t fd){
	/* Sanity check */
	if(fd < 0 || fd > 7)
		return -1;

	/* If file is already closed, return -1 */
	if(file_desc[fd].flag == 0)
		return -1;
	
	/* Clean up file desc array entry */
	file_desc[fd].f_op = NULL;
	file_desc[fd].inode = 0;
	file_desc[fd].f_pos = 0;
	file_desc[fd].flag = 0;

	/* Return 0 on success */
	return 0;	
}













