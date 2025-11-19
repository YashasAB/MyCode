#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "terminal.h"
#include "filesystem.h"

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

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 *       Inputs: None
 *      Outputs: PASS/FAIL
 * Side Effects: None
 *     Coverage: Load IDT, IDT definition
 *        Files: x86_desc.h/S
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

/* Checkpoint 1 tests */

int test_divide_error_ex() {
    /* tests the divide error exception */
    int a = 0;
    int b = 1/a;
    printf("answer is: %d\n", b);
		return b;
}

/*
 * PAGING Test
 * Test the memory locations allocated in paging.c
 * including edge cases
 * outputs: PASS/FAIL
 * inputes:none
 *  files: paging.c/h
 */
int paging_test(){
	TEST_HEADER;
	int result = PASS;
	uint32_t i;
	uint32_t *ptr;
	ptr =(uint32_t*) 0xb8000;
	i = *ptr;
	ptr = ptr +4;
	i = *ptr;

	ptr = (uint32_t*)0x400000 ;
	i = *ptr;
	ptr = ptr +4;
	i = *ptr;
	ptr = (uint32_t*)(0x800000 -4);
	i = *ptr;

	return result;
}

/*
 * page_fault_test
 * dereference a null ptr shoud generate page_fault
 * outputs: PASS/FAIL
 * inputes:none
 * files: idt.c
 */
int page_fault_test(){
	TEST_HEADER;
	int result = PASS;
	uint32_t i;
	uint32_t *ptr;
	ptr = (uint32_t *) 0x0;
	i = *ptr;
	return result;

}

/* Checkpoint 2 tests */

/*
 * rtc_test
 * DESCRIPTION: Checks if RTC works as intended
 *       INPUT: NONE
 *      RETURN: PASS
 */
int32_t rtc_test(void) {
	int32_t fd;
	int32_t buf;
	int32_t nbytes;
	int32_t i = 0;
	int32_t speed = 2;
	uint8_t *filename;
	
	clear();
	rtc_open(filename);
	while(i < 100) 
	{
		rtc_read(fd, &buf, nbytes);
		test_interrupts();
		if((i % 10) == 0) 
		{
			rtc_write(fd,&speed,nbytes);
			if(speed <= 1024)
			{
				speed *= 2;
			}
		}
		i++;
	}
	rtc_close(fd);
	clear();
	return PASS;
}

/*
 * rtc_write_fail_test
 * DESCRIPTION: Checks if RTC interrupt freq will be changed by an
 *              erroneous value
 *       INPUT: NONE
 *      RETURN: PASS if rtc_write is working, FAIL if not
 */
int32_t rtc_write_fail_test(void) {
	int32_t fd;
	int32_t buf;
	int32_t nbytes;
	int32_t i = 0;
	uint8_t * filename;
	int32_t if_written;
	
	clear();
	rtc_open(filename);
	while(i < 5) 
	{
		rtc_read(fd, &buf, nbytes);
		test_interrupts();
		i++;
	}
	buf = 33;
	if_written = rtc_write(fd, &buf, nbytes);
	while(i < 10) 
	{
		rtc_read(fd,&buf,nbytes);
		test_interrupts();
		i++;
	}
	rtc_close(fd);
	clear();
	if(if_written == -1) 
	{
		return PASS;
	}
	return FAIL;
}

/*
 * default_terminal_test
 * DESCRIPTION: Tests keyboard and terminal interaction. The test
 *              will not exit the while loop as it stands.
 *       INPUT: NONE
 *      RETURN: PASS
 */
int32_t default_terminal_test(void) 
{
	int32_t fd;
	int8_t buf[128];
	int32_t nbytes = 128;
	uint8_t *filename;
	terminal_open(filename);
	while(1) 
	{
		nbytes = terminal_read(fd, buf, 128);
		terminal_write(fd, buf, nbytes);
	}
	terminal_close(fd);
	return PASS;
}

/*
 * terminal_read_less_bytes
 * DESCRIPTION: Tests special case where read parameters for
 *              terminal_read is less than 128 bytes
 *       INPUT: NONE
 *      RETURN: PASS after hitting enter
 */
int32_t terminal_read_less_bytes(void) 
{
	int32_t fd;
	int8_t buf[4];
	int32_t nbytes = 4;
	uint8_t *filename;
	terminal_open(filename);
	while(nbytes > 1) 
	{
		nbytes = terminal_read(fd,buf,4);
		terminal_write(fd,buf,nbytes);
	}
	terminal_close(fd);
	return PASS;
}

/*
 * dir_read_test
 * DESCRIPTION: Tests if dir_read works
 *       INPUT: NONE
 *      RETURN: PASS
 */
int32_t dir_read_test(void) {
	uint8_t * filename;
	int32_t fd;
	int8_t buf[32];
	int32_t nbytes = 32;
	dir_open(filename);
	while(0 != (nbytes = dir_read(fd,buf,nbytes))) {
		terminal_write(fd,buf,nbytes);
		putc('\n');
	}
	dir_close(fd);
	return PASS;
}


/*
 * fs_read_small_file
 * DESCRIPTION: Tests that demonstrates that you can read small files 
 *       INPUT: None
 *      RETURN: 0
 */
int fs_read_small_file()
{
	int32_t fd;
	file_open((uint8_t*) "frame0.txt");
	char buf[500];
	int result = file_read(0, buf, 500);
	terminal_write(0, buf, result);
	file_close(fd);
	return PASS;
}

/*
 * fs_read_large_file
 * DESCRIPTION: Tests that demonstrates that you can read large files 
 *       INPUT: None
 *      RETURN: 0
 */
int fs_read_large_file()
{
	int32_t fd;
	file_open((uint8_t*) "verylargetextwithverylongname.txt");

	// size here is base off of the know file sizes, which is 
	// over 5 KB
	char buf[6000];
	int result = file_read(0, buf, 6000);
	terminal_write(0, buf, result);
	file_close(fd);
	return PASS;
}

/*
 * fs_read_exec_file
 * DESCRIPTION: Tests that demonstrates the results for read executable files 
 * 				ELF is visible at the start of the write
 *       INPUT: None
 *      RETURN: 0
 */
int fs_read_exec_file()
{
	int32_t fd;
	file_open((uint8_t*) "pingpong");
	char buf[10000];
	int result = file_read(0, buf, 10000);
	terminal_write(0, buf, result);
	file_close(fd);
	return PASS;
}

/*
 * fs_read_exec_file
 * DESCRIPTION: Tests that demonstrates the results for read non-existent files
 * 				Nothing should show up if implemented correctly
 *       INPUT: None
 *      RETURN: 0
 */
int fs_read_fail()
{
	int32_t fd;
	file_open((uint8_t*) "tuxPenguins.txt");		// non-existent file
	char buf[500];
	int result = file_read(0, buf, 500);
	terminal_write(0, buf, result);
	file_close(fd);
	if(result == FAILURE)
		return PASS;
	return FAILURE;
}

/*
 * system_call_handler_test
 * DESCRIPTION: Tests to see if handler works correctly
 *       INPUT: None
 *      RETURN: 0
 */
int system_call_handler_test()
{	
	asm volatile (
      "movl $3,%%eax ; "
			"int  $128     ; "
       :
       : 
       :"memory", "cc", "eax"

    );
	return 0;
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests() {
	// TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here

	/* Checkpoint 1 tests */

	// TEST_OUTPUT("paging_test",paging_test());
	// TEST_OUTPUT("page_fault_test", page_fault_test());
	// TEST_OUTPUT("INT0_test",test_divide_error_ex())

	/* Checkpoint 2 tests */

	// TEST_OUTPUT("RTC_test", rtc_test());
	// TEST_OUTPUT("RTC_write_fail_test", rtc_write_fail_test());
	// TEST_OUTPUT("terminal_test", default_terminal_test());
	// TEST_OUTPUT("terminal_read_less_bytes",terminal_read_less_bytes());
	// TEST_OUTPUT("dir_read_test",dir_read_test());
	// TEST_OUTPUT("fs_test_small", fs_read_small_file());
	// TEST_OUTPUT("fs_test_large", fs_read_large_file());
	// TEST_OUTPUT("fs_test_exec", fs_read_exec_file());
	// TEST_OUTPUT("fs_fail_test", fs_read_fail());
	system_call_handler_test();
}
