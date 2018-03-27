#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#define PASS 1
#define FAIL 0
#define FILE_NAME_LENGTH 32
#define FILE_COUNT 17
#define FILE_HEAD_LENGTH 24
#define FILE_END_LENGTH 50

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}

/* Temprary file descriptor array */
file_desc_t file_desc[8];

static int RTC_test();
static int test_dir_file();

/* Checkpoint 1 tests */

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/*
 * valid_page_test:
 * Description: Test paging for valid address
 * Input: None
 * Output: return PASS on success, Page Fault on fail
 * Effect: Test paging on valid address:
 * 	4MB kernel page,
 * 	4KB video memory page
 */
int valid_page_test(){
	TEST_HEADER;

	/* Define local variables */
	int i;
	int result = PASS;

	/* Test on 4MB Kernel Page*/
	unsigned long* addr = (unsigned long*)0x400000;
	unsigned long temp;
	for(i = 0; i < 0x100000; ++i){
		temp = *(addr + i);
	}

	/* Print out message */
	printf("4MB test passed\n");

	/* Test on 4KB video memory Page*/
	addr = (unsigned long*)0xb8000;
	for(i = 0; i <0x400; i++){
		temp = *(addr + i);
	}

	/* Print out message */
	printf("4KB test passed\n");

	return result;
}
/*  div_by_0_test
 *  Test division by 0 will trigger div by 0 handler
 *  If successful will be stuck in handler and not return
 */
int div_by_0_test(){
    TEST_HEADER;
    int i=7;
    int j=2;
    int k;
    for(k=0;k<2;k++){
        j=j/2;
    }
    //j should b 0 here
    i=i/j;
    return FAIL;
}
/*
 * invalid_page_test:
 * Description: Test paging for invalid address
 * Input: None
 * Output: Page Fault on success, will return FAIL if page fault not triggered
 * Effect: Test paing on invalid address 0x0
 */
int invalid_page_test(){
	TEST_HEADER;

	/* Initialize addr to 0x0 as an invalid addr */
	unsigned long* addr = (unsigned long*)0x0;
	unsigned long temp;

	/* Dereference invalid addr to cause Page Fault */
	temp = *addr;

	return FAIL;
}

/* Checkpoint 2 tests */

/*
 * test_file_by_name:
 * Description: Test on operations on a file given name
 * Input: File name of a text file
 * Ouput: Result of the test
 * Effect: Print out data files and run corresponding test for other files
 */
int test_file_by_name(const int8_t* fname){
	TEST_HEADER;

	/* Initialize variables */
	int i, fd;
	uint32_t size;
	dentry_t dentry;

	/* Clear file_name */
	for(i = 0; i < FILE_NAME_LENGTH; i++)
		dentry.file_name[i] = '\0';

	/* Fill dentry for info */
	if(read_dentry_by_name(fname, &dentry) == -1)
		return FAIL;

	/* Call corresponding test for non data file */
	if(dentry.file_type == 0)
		return RTC_test();
	else if(dentry.file_type == 1)
		return test_dir_file();

	/* Get size for text file */
	size = get_size(fname);
	int8_t buf[size];

	/* Assume size is larger than 10 */
	if(size < 10)
		return FAIL;

	/* Open text file */
	if(data_open(fname) != 0)
		return FAIL;

	/* Reserve file descriptor 0 and 1 for stdin and stdout, search for free entry */
	for(i = 2; i < 8; i++){
		/* If free, then fill in file_desc_t */
		if(file_desc[i].flag == 0){
			file_desc[i].inode = dentry.inode;
			file_desc[i].f_pos = 0;
			file_desc[i].flag = 1;
			/* Change fd */
			fd = i;
			break;
		}
	}

	/* Sanity check */
	if(fd < 0 || fd > 7)
		return FAIL;

	/* Read first 10 bytes */
	if(data_read(fd, (int8_t*)buf, 10) != 10)
		return FAIL;

	/* Write function should be disabled */
	if(data_write(fd, (int8_t*)buf, 10) != -1)
		return FAIL;

	/* Print out first 10 bytes */
	for(i = 0; i < 10; i++)
		putc(buf[i]);

	/* Read the rest of text file */
	if(data_read(fd, (int8_t*)buf, size - 10) != (size - 10))
		return FAIL;

	/* Print out rest of text file */
	for(i = 0; i < (size - 10); i++)
		putc(buf[i]);

	/* Anymore read should return 0 for EOF */
	if(data_read(fd, (int8_t*)buf, 1) != 0)
		return FAIL;

	/* Print out file name for reference */
	printf("\nFile Name: ");
	for(i = 0; i < FILE_NAME_LENGTH; i++){
		if(*(fname + i) == '\0')
			break;
		printf("%c", *(fname + i));
	}
	printf("\n");

	/* Close the text file */
	if(data_close(fd) != 0)
		return FAIL;

	return PASS;
}

/*
 * test_file_by_index:
 * Description: Test on operations of a file given index
 * Input: Index into the boot block
 * Ouput: Result of the test
 * Effect: Print out data files and run corresponding test for other files
 */
int test_file_by_index(uint32_t index){
	TEST_HEADER;

	/* Initialize variables */
	int i, fd;
	uint32_t size;
	dentry_t dentry;

	/* Clear file_name */
	for(i = 0; i < FILE_NAME_LENGTH; i++)
		dentry.file_name[i] = '\0';

	/* Fill dentry for info */
	if(read_dentry_by_index(index, &dentry) == -1)
		return FAIL;

	/* Call corresponding test for non data file */
	if(dentry.file_type == 0)
		return RTC_test();
	else if(dentry.file_type == 1)
		return test_dir_file();

	/* Get size for text file */
	size = get_size((int8_t*)dentry.file_name);
	int8_t buf[size];

	/* Assume size is larger than 10 */
	if(size < 10)
		return FAIL;

	/* Open text file */
	if(data_open((int8_t*)dentry.file_name) != 0)
		return FAIL;

	/* Reserve file descriptor 0 and 1 for stdin and stdout, search for free entry */
	for(i = 2; i < 8; i++){
		/* If free, then fill in file_desc_t */
		if(file_desc[i].flag == 0){
			file_desc[i].inode = dentry.inode;
			file_desc[i].f_pos = 0;
			file_desc[i].flag = 1;
			/* Change fd */
			fd = i;
			break;
		}
	}

	/* Sanity check */
	if(fd < 0 || fd > 7)
		return FAIL;

	/* Read first 10 bytes */
	if(data_read(fd, (int8_t*)buf, 10) != 10)
		return FAIL;

	/* Write function should be disabled */
	if(data_write(fd, (int8_t*)buf, 10) != -1)
		return FAIL;

	/* Print out first 10 bytes */
	for(i = 0; i < 10; i++)
		putc(buf[i]);

	/* Read the rest of text file */
	if(data_read(fd, (int8_t*)buf, size - 10) != (size - 10))
		return FAIL;

	/* Print out rest of text file */
	for(i = 0; i < (size - 10); i++)
		putc(buf[i]);

	/* Anymore read should return 0 for EOF */
	if(data_read(fd, (int8_t*)buf, 1) != 0)
		return FAIL;

	/* Print out file name for reference */
	printf("\nFile Name: ");
	for(i = 0; i < FILE_NAME_LENGTH; i++){
		if(dentry.file_name[i] == '\0')
			break;
		printf("%c", dentry.file_name[i]);
	}
	printf("\n");

	/* Close the text file */
	if(data_close(fd) != 0)
		return FAIL;

	return PASS;
}
/*
 * test_dir_file:
 * Description: Test on reading a directory
 * Input: None
 * Output: Test result
 * Effect: Print out all files in the directory with info
 */
int test_dir_file(){
	TEST_HEADER;

	/* Initialize variables */
	int i, j, fd;
	int8_t buf[FILE_NAME_LENGTH];
	dentry_t dentry;

	/* Fill dentry for info */
	read_dentry_by_name(".", &dentry);

	/* Open the . directory */
	if(dir_open(".") != 0)
		return FAIL;
	
	/* Reserve file descriptor 0 and 1 for stdin and stdout, search for free entry */
	for(i = 2; i < 8; i++){
		/* If free, then fill in file_desc_t */
		if(file_desc[i].flag == 0){
			file_desc[i].inode = dentry.inode;
			file_desc[i].f_pos = 0;
			file_desc[i].flag = 1;
			/* Change fd */
			fd = i;
			break;
		}
	}

	/* Sanity check */
	if(fd < 0 || fd > 7)
		return FAIL;

	/* Write function should be disabled */
	if(dir_write(fd, (int8_t*)buf, FILE_NAME_LENGTH) != -1)
		return FAIL;

	
	/* Print out file names and info for every file */
	for(j = 0; j < FILE_COUNT; j++){
		/* Read in file name */
		if(dir_read(fd, (int8_t*)buf, FILE_NAME_LENGTH) != FILE_NAME_LENGTH)
			return FAIL;

		/* Fill in dentry for info */
		read_dentry_by_index(j, &dentry);
		
		/* Print out file name and info */
		printf("Name: ");
		for(i = 0; i < FILE_NAME_LENGTH; i++)
			printf("%c", buf[i]);

		printf(" Type: %d Size: %d\n", dentry.file_type, get_size((int8_t*)buf));
	}

	/* Anymore read should return 0 for no more file to read */
	if(dir_read(fd, (uint8_t*)buf, FILE_NAME_LENGTH) != 0)
		return FAIL;

	/* Close the directory */
	if(data_close(fd) != 0)
		return FAIL;

	return PASS;

}

/*
 * test_head_tail_data_file:
 * Description: Test on binary files
 * Input: File name of a binary file
 * Output: Test result
 * Effect: Print out the first 24B and last 50B and file name
 */
int test_head_tail_data_file(const int8_t* fname){
	TEST_HEADER;

	/* Initialize variables */
	int i, fd;
	uint32_t size;
	int8_t start[FILE_HEAD_LENGTH];
	int8_t end[FILE_END_LENGTH];
	dentry_t dentry;	

	/* Get size for the binary file */
	size = get_size(fname);
	int8_t buf[size];

	/* Fill dentry for info */
	read_dentry_by_name(fname, &dentry);

	/* Assume size is larger than 74B */
	if(size < (FILE_HEAD_LENGTH + FILE_END_LENGTH))
		return FAIL;

	/* Open the binary file */
	if(data_open(fname) != 0)
		return FAIL;

	/* Reserve file descriptor 0 and 1 for stdin and stdout, search for free entry */
	for(i = 2; i < 8; i++){
		/* If free, then fill in file_desc_t */
		if(file_desc[i].flag == 0){
			file_desc[i].inode = dentry.inode;
			file_desc[i].f_pos = 0;
			file_desc[i].flag = 1;
			/* Change fd */
			fd = i;
			break;
		}
	}

	/* Sanity check */
	if(fd < 0 || fd > 7)
		return FAIL;

	/* Read in first 24B for print */
	if(data_read(fd, (int8_t*)start, FILE_HEAD_LENGTH) != FILE_HEAD_LENGTH)
		return FAIL;

	/* Read in middle of the file */
	if(data_read(fd, (int8_t*)buf, (size - FILE_HEAD_LENGTH - FILE_END_LENGTH)) != (size - FILE_HEAD_LENGTH - FILE_END_LENGTH))
		return FAIL;

	/* Write should be disabled */
	if(data_write(fd, (int8_t*)buf, (size - FILE_HEAD_LENGTH - FILE_END_LENGTH)) != -1)
		return FAIL;

	/* Read in the last 50B for print */
	if(data_read(fd, (int8_t*)end, FILE_END_LENGTH) != FILE_END_LENGTH)
		return FAIL;

	/* Anymore read should return 0 for EOF */
	if(data_read(fd, (int8_t*)buf, 1) != 0)
		return FAIL;

	/* Print out file name for reference */
	printf("Name: ");
	for(i = 0; i < FILE_NAME_LENGTH; i++){
		if(*(fname + i) == '\0')
			break;
		printf("%c", *(fname + i));
	}
	
	/* Print out first 24B for check */
	printf("\nStart: ");
	for(i = 0; i < FILE_HEAD_LENGTH; i++)
		putc(start[i]);
	
	/* Print out last 50B for check */
	printf("\nEnd: ");
	for(i = 0; i < FILE_END_LENGTH; i++)
		putc(end[i]);
	printf("\n");

	/* Close the binary file */
	if(data_close(fd) != 0)
		return FAIL;

	return PASS;
}
/* RTC_test
 * DESCRIPTION: Test changing RTC rate
 * INPUT: None
 * OUTPUT: PASS as success (complete = success)
 */
int RTC_test(){
    TEST_HEADER;
    //initialize_RTC();
    rtc_open(0);

    int i, j;
    char output;
    int freq = 2;
    int* ptr = &freq;

    // print rate char in different rates
    for(i = 0; i < 10; i++){
        for(j = 0; j < 40; j++){
            output = ('0'+i);
            putc(output);
            rtc_read(0,0,0);
        }
        freq <<= 1;
        rtc_write(0, (void*) ptr, 4);
        putc('\n');
    }

    //test invalid frequencies
    freq <<=1;//more than 1024
    if(!rtc_write(0, (void*) ptr, 4))return FAIL;
    freq = 1;//less than 2
    if(!rtc_write(0, (void*) ptr, 4))return FAIL;
    freq = 3;//not power of 2
    if(!rtc_write(0, (void*) ptr, 4))return FAIL;

    rtc_close(0);

    puts("\nRTC TEST COMPLETE\n");
    return PASS;

}
/* stdin_read_test
 * tests stdin read call. Check if last character in buffer is \n
 */
int stdin_read_test(){
    TEST_HEADER;
    #define bufsize 128 //size of stdin buffer
    char buffer[bufsize]={0};
    stdin_open(0);
    while(1){
        int nchar=stdin_read(0,buffer,bufsize);
        printf("size: %d",nchar);
        int i;
        printf("stdin_read: ");
        for(i=0;i<nchar;i++)putc(buffer[i]);
        if(buffer[nchar-1]!='\n'){
            stdin_close(0);
            return FAIL;
        }
    }
    stdin_close(0);
    return PASS;
#undef bufsize
}
/* long_printf_test
 * test terminal printing and scrolling functionality
 */
int long_printf_test(){
    TEST_HEADER;
    #define PAUSE for(count=0;count<4;count++)while(rtc_read(0,0,0))
    uint32_t freq=2;
    int count=0;
    rtc_open(0);
    rtc_write(0,&freq,sizeof(uint32_t));

    printf("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPRSTUVWXYZ1234567890`~!@#$%^&*()-=_+[]\\|;:'\",./<>?the quick brown fox jumps over the lazy dog\n");
    //scrolling test
    printf("the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= the quick brown fox jumps over the lazy dog 1234567890-= ");
    PAUSE;

    printf("\ntesting puts, check if cursor moves with print. \nWill pause for 2 seconds for each line");



    puts("\n1the quick brown fox jumps over the lazy dog 1234567890-=");
    PAUSE;
    puts("\n2the quick brown fox jumps over the lazy dog 1234567890-=");
    PAUSE;
    puts("\n3the quick brown fox jumps over the lazy dog 1234567890-=");
    PAUSE;
    puts("\n4the quick brown fox jumps over the lazy dog 1234567890-=");
    PAUSE;
    rtc_close(0);
    putc('\n');

    printf("non printable char\n");
    int c=0;
    for(c=0;c<' ';c++){
        putc(c);
    }
    printf("\nextended ascii\n");
    for(c=0x80;c<0xFF;c++){
        putc(c);
    }
    putc(c);
    putc('\n');
    return PASS;
}
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
    //cp1 tests
    //TEST_OUTPUT("idt_test", idt_test());
    //TEST_OUTPUT("valid_page_test", valid_page_test());
    //cp1 crashing tests
#if DIV_0_TEST
    TEST_OUTPUT("div_by_0_test", div_by_0_test());
#endif
#if INVALID_ADDR_TEST
    TEST_OUTPUT("invalid_page_test", invalid_page_test());
#endif

    //cp2 tests
    /* test file by name */
    //TEST_OUTPUT("Data File Test", test_file_by_name("."));
    //TEST_OUTPUT("Data File Test", test_file_by_name("rtc"));
    //TEST_OUTPUT("Data File Test", test_file_by_name("hello"));
    //TEST_OUTPUT("Data File Test", test_file_by_name("frame0.txt"));
    //TEST_OUTPUT("Data File Test", test_file_by_name("frame1.txt"));
    //TEST_OUTPUT("Data File Test", test_file_by_name("created.txt"));
    //TEST_OUTPUT("Data File Test", test_file_by_name("verylargetextwithverylongname.tx"));
    //TEST_OUTPUT("Data File Test (oversize filename, should fail)", test_file_by_name("verylargetextwithverylongname.txt"));
    /* test file by index */
    //TEST_OUTPUT("Data File Test", test_file_by_index(0));
    //TEST_OUTPUT("Data File Test", test_file_by_index(1));
    //TEST_OUTPUT("Data File Test", test_file_by_index(2));
    //TEST_OUTPUT("Data File Test", test_file_by_index(3));
    //TEST_OUTPUT("Data File Test", test_file_by_index(5));
    //TEST_OUTPUT("Data File Test", test_file_by_index(10));
    //TEST_OUTPUT("Data File Test", test_file_by_index(11));

    /* other file tests */
    //TEST_OUTPUT("Directory File Test", test_dir_file());
    //TEST_OUTPUT("Non Text File Test", test_head_tail_data_file("hello"));

    /* other tests */
    //TEST_OUTPUT("Long printf test (terminal driver)", long_printf_test());
    //TEST_OUTPUT("RTC_test", RTC_test());
    TEST_OUTPUT("stdin read test", stdin_read_test());
}
