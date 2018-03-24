#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#define PASS 1
#define FAIL 0

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

/* RTC_test
 * Test changing RTC rate
 * Inputs: None
 * Side Effects: None

void RTC_test(){
	TEST_HEADER;
	open_RTC();

	int i, j;
	char output;
	int freq = 2;
	int* ptr = &freq;

	for(i = 0; i < 10; i++){
		for(j = 0;j < 20; j++){
			output = ('0'+i);
			putc(output);
			read_RTC();
		}
		freq <<= 1;
		write_RTC(0, (void*) ptr, 4);
		putc('\n');
	}

	close_RTC();
	for(i = 0; i < 20; i++){
		putc(output);
		read_RTC();
	}
	puts("\nRTC TEST COMPLETE\n");

}
*/

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	TEST_OUTPUT("valid_page_test", valid_page_test());
#if DIV_0_TEST
	TEST_OUTPUT("div_by_0_test", div_by_0_test());
#endif
#if INVALID_ADDR_TEST
	TEST_OUTPUT("invalid_page_test", invalid_page_test());
#endif
}
