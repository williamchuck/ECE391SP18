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

// add more tests here

int valid_page_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	unsigned long* addr = (unsigned long*)0x400000;
	unsigned long temp;
	for(i = 0; i < 0x100000; ++i){
		temp = *(addr + i);
	}

	printf("4MB test passed\n");	

	addr = (unsigned long*)0xb8000;
	for(i = 0; i <0x400; i++){
		temp = *(addr + i);
	}

	printf("4KB test passed\n");

	return result;
}

int invalid_page_test(){
	TEST_HEADER;

	unsigned long* addr = (unsigned long*)0x0;
	unsigned long temp;

	temp = *addr;
	
	return PASS;
}
/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	TEST_OUTPUT("valid_page_test", valid_page_test());
#if INVALID_ADDR_TEST
	TEST_OUTPUT("invalid_page_test", invalid_page_test());
#endif
	// launch your tests here
}
