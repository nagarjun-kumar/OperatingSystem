#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "fs.h" /* Include the file system supporter */
#include "rtc.h"
#include "keyboard.h"
#include "terminal.h"
#include "syscall.h"

#define PASS 1
#define FAIL 0

/* Specific addresses for testing */
#define KERNAL_START_ADD   0xB8000
#define RANDOM_KERNAL_ADD  0xB8567
#define KERNAL_END_ADD     0xBFFFF
#define VIDEO_MEM_START    0x400000
#define RANDOM_VIDEO_MEM   0x567890
#define VIDEO_MEM_END      0x7FFFFF

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

/* int page_test_valid_address_1()
 * Description: Test if a pointer to a valid address(start of kermal mem) is valid
 * Inputs: None
 * Outputs: Test result. Pass/ Fail(Error thrown)
 * Returned Value: PASS upon success	
 * Side Effect:  Throw PAGE FAULT exception when derefrencing is invalid
 * Expected outcome: Pass
 */
int page_test_valid_address_1() {
	TEST_HEADER;
	uint8_t* ptr = (uint8_t*)(KERNAL_START_ADD);
	uint8_t value_deref;
	value_deref = *ptr;
	return PASS;
}
/* int page_test_valid_address_2()
 * Description: Test if a pointer to a valid address(end of kermal mem) is valid
 * Inputs: None
 * Outputs: Test result. Pass/ Fail(Error thrown)
 * Returned Value: PASS upon success	
 * Side Effect:  Throw PAGE FAULT exception when derefrencing is invalid
 * Expected outcome: Pass
 */
int page_test_valid_address_2() {
	TEST_HEADER;
	uint8_t* ptr = (uint8_t*)(KERNAL_END_ADD);
	uint8_t value_deref;
	value_deref = *ptr;
	return PASS;
}
/* int page_test_valid_address_3()
 * Description: Test if a pointer to a valid kernal address(middle of kermal mem) is valid
 * Inputs: None
 * Outputs: Test result. Pass/ Fail(Error thrown)
 * Returned Value: PASS upon success	
 * Side Effect:  Throw PAGE FAULT exception when derefrencing is invalid
 * Expected outcome: Pass
 */
int page_test_valid_address_3() {
	TEST_HEADER;
	uint8_t* ptr = (uint8_t*)(RANDOM_KERNAL_ADD);
	uint8_t value_deref;
	value_deref = *ptr;
	return PASS;
}
/* int page_test_valid_address_4()
 * Description: Test if a pointer to a valid address(start of video mem) is valid
 * Inputs: None
 * Outputs: Test result. Pass/ Fail(Error thrown)
 * Returned Value: PASS upon success	
 * Side Effect:  Throw PAGE FAULT exception when derefrencing is invalid
 * Expected outcome: Pass
 */
int page_test_valid_address_4() {
	TEST_HEADER;
	uint8_t* ptr = (uint8_t*)(VIDEO_MEM_START);
	uint8_t value_deref;
	value_deref = *ptr;
	return PASS;
}
/* int page_test_valid_address_5()
 * Description: Test if a pointer to a valid address(end of video mem) is valid
 * Inputs: None
 * Outputs: Test result. Pass/ Fail(Error thrown)
 * Returned Value: PASS upon success	
 * Side Effect:  Throw PAGE FAULT exception when derefrencing is invalid
 * Expected outcome: Pass
 */
int page_test_valid_address_5() {
	TEST_HEADER;
	uint8_t* ptr = (uint8_t*)(VIDEO_MEM_END);
	uint8_t value_deref;
	value_deref = *ptr;
	return PASS;
}
/* int page_test_valid_address_6()
 * Description: Test if a pointer to a valid address(middle of video mem) is valid
 * Inputs: None
 * Outputs: Test result. Pass/ Fail(Error thrown)
 * Returned Value: PASS upon success	
 * Side Effect:  Throw PAGE FAULT exception when derefrencing is invalid
 * Expected outcome: Pass
 */
int page_test_valid_address_6() {
	TEST_HEADER;
	uint8_t* ptr = (uint8_t*)(RANDOM_VIDEO_MEM);
	uint8_t value_deref;
	value_deref = *ptr;
	return PASS;
}
/* int page_test_invalid_address_1()
 * Description: Test if a pointer to an invalid kernal address(after end of kermal mem) is valid
 * Inputs: None
 * Outputs: Test result. Pass/ Fail(Error thrown)
 * Returned Value: PASS upon success	
 * Side Effect:  Throw PAGE FAULT exception when derefrencing is invalid
 * Expected outcome: Failure
 */
int page_test_invalid_address_1() {
	TEST_HEADER;
	uint8_t* ptr = (uint8_t*)(KERNAL_END_ADD + 1);
	uint8_t value_deref;
	value_deref = *ptr;
	return PASS;
}
/* int page_test_invalid_address_2()
 * Description: Test if a pointer to an invalid address(after end of video mem) is valid
 * Inputs: None
 * Outputs: Test result. Pass/ Fail(Error thrown)
 * Returned Value: PASS upon success	
 * Side Effect:  Throw PAGE FAULT exception when derefrencing is invalid
 * Expected outcome: Failure
 */
int page_test_invalid_address_2() {
	TEST_HEADER;
	uint8_t* ptr = (uint8_t*)(VIDEO_MEM_END + 1);
	uint8_t value_deref;
	value_deref = *ptr;
	return PASS;
}
/* int page_test_invalid_address_3()
 * Description: Test if a pointer to an invalid address(before start of video mem) is valid
 * Inputs: None
 * Outputs: Test result. Pass/ Fail(Error thrown)
 * Returned Value: PASS upon success	
 * Side Effect:  Throw PAGE FAULT exception when derefrencing is invalid
 * Expected outcome: Failure
 */
int page_test_invalid_address_3() {
	TEST_HEADER;
	uint8_t* ptr = (uint8_t*)(VIDEO_MEM_START - 1);
	uint8_t value_deref;
	value_deref = *ptr;
	return PASS;
}
/* int presence_test()
 * Description: Test if the presence bits in pde's and pte's are set correctly
 * Inputs: None
 * Outputs: Test result. Pass/ Fail(Error thrown)
 * Returned Value: PASS upon success	
 * Side Effect:  None
 * Expected outcome: Pass
 */
int paging_struct_test(){
	TEST_HEADER;
	if(page_directory[0].present_4kb == 0 || page_directory[1].present_4mb == 0){
		printf("\n Video momeory or kernel momory not presented \n");
		return FAIL;
	}
	if(page_table[1].present == 0 || page_table[123].present == 0 || page_table[789].present == 0){
		printf("\n Part of page table missing \n");
		return FAIL;
	}
	return PASS;
}
/*divided_by_zero_test()
*test exception divided by zero
*input:none
*output:none
*return value:none
*side effect: stop kernel
*/
void divided_by_zero_test(){
	TEST_HEADER;
	int a;
	int b = 1;
	int c = 0;
	a = b/c;
}

/* null_test()
* derefrencing null causing page fault
*input : none
*output: none
*return value: none
*side effect: stop kernel
*/
void null_test(){
	TEST_HEADER;
	int* ptr = NULL;
	int a;
	a = *(ptr);
}

/* Checkpoint 2 tests */
void rtc_test() {
	int count;
	char freq;
	printf("starting rtc test\n");
	printf("start testing rtc_open(), frequency = 2Hz\n");
	count = 20;
	rtc_open(0, 0);
	while(count > 0) {
		rtc_read(0, 0, 0, 0);
		printf("hello! ");
		count--;
	}
	clear();
	reset_cursor();
	printf("start testing rtc_write, frequency = 8Hz\n");
	freq = 8;
	rtc_write(0,0,&freq,BYTE);
	count = 50;
	while(count > 0) {
		rtc_read(0,0,0,0);
		printf("hello! ");
		count--;
	}
	clear();
	reset_cursor();
	printf("start testing rtc_write, frequency = 32Hz\n");
	freq = 32;
	rtc_write(0,0,&freq,BYTE);
	count = 50;
	while(count > 0) {
		rtc_read(0,0,0,0);
		printf("hello! ");
		count--;
	}
}

void terminal_write_test() {
	int8_t buf[6] = {'h', 'e', 'l', 'l', 'o', '!'};
	terminal_write(NULL,0, buf, 6);
}

void print_buffer(char* buf, int32_t size) {
	int i;
	for(i = 0; i < size; i++) {
		putc(buf[i], visible_terminal);
	}
}
void terminal_read_test() {
	printf("start testing terminal read\n");
	uint32_t nbytes;
	nbytes = 0;
	int32_t buf_size;
	char buf[128];
	buf_size = terminal_read(0,0, buf, nbytes);
	print_buffer(buf, buf_size);
}
void terminal_test() {
	printf("System will echo the letters you type after you hit enter\n");
	int32_t buf_size;
	char buf[128];
	buf_size = terminal_read(NULL,NULL, buf, 0);
	terminal_write(NULL, NULL,buf, buf_size);
}
//wait for enter key to proceed
void wait_for_enter() {
	term[visible_terminal].enter_flag = 0;
	while(term[visible_terminal].enter_flag == 0) {}
	term[visible_terminal].enter_flag = 0;
	return;
}
/* int initialization_test()
 * Description: Test if the file system has been initialized properly
 * Inputs: None
 * Outputs: Test result. Pass/ Fail
 * Returned Value: PASS upon success	
 * Side Effect:  None
 * Expected outcome: Pass
 */
 int initialization_test() {
	 if (fs_initialized() == FS_SUCCESS) { /* Call helper function to determine if succeeded */
		 return PASS;
	 }
	 return FAIL;
 }

/* int read_small_existent_file_test()
 * Description: Test if the file system reads small existing file correctly
 * Inputs: None
 * Outputs: Test result. Pass/ Fail
 * Returned Value: PASS upon success	
 * Side Effect:  None
 * Expected outcome: Pass
 */
 int read_small_existent_file_test() {
	 dentry_t ref; /* Temporary variable to store file information */
	 int32_t read_result; /* Flag to see if read succeeds */
	 int32_t num_bytes_read; /* # bytes read in the file */
	 char buf[256]; /* A buf with size larger than that of the file */
	 int loop_idx; /* Loop index */
	 read_result = read_dentry_by_name("frame1.txt", &ref); /* Try to read directory entry */
	 if (read_result == FS_FAILURE) {
		 return FAIL;
	 }
	 if (fs_inode_length(ref.inode_num) != 174) { /* Check if length of file is calculated correctly */
		return FAIL;
	 }
	 num_bytes_read = read_data(ref.inode_num, 0, buf, 256); /* Try to fill the 256-byte buf */
	 if (num_bytes_read != 174) { /* Check if there are indeed 174 bytes read */
		return FAIL;
	 }
	 for (loop_idx = 0; loop_idx < 174; loop_idx++) { /* Loop through filled buf */
		putc(buf[loop_idx], visible_terminal); /* print characters */
	 }
	 return PASS;
 }

 /* int read_existent_file_test_2()
 * Description: Test if the file system reads long file (> 1 data block) correctly
 * Inputs: None
 * Outputs: Test result. Pass/ Fail
 * Returned Value: PASS upon success	
 * Side Effect:  None
 * Expected outcome: Pass
 */
 int read_existent_file_test_2() {
	 dentry_t ref; /* Temporary variable to store file information */
	 int32_t read_result; /* Flag to see if read succeeds */
	 char buf[10000]; /* A buf with size similar to data block */
	 int loop_idx; /* Loop index */
	 read_result = read_dentry_by_name("ls", &ref); /* Try to read directory entry */
	 if (read_result == FS_FAILURE) {
		 return FAIL;
	 }
	 read_data(ref.inode_num, 0, buf, 10000);
	 for (loop_idx = 0; loop_idx < fs_inode_length(ref.inode_num); loop_idx++) { /* Loop through filled buf */
	 	if (buf[loop_idx] == NULL) { /* Not printing NULL bytes */
			 continue;
		 }
		putc(buf[loop_idx], visible_terminal); /* print characters */
	 }
	 return PASS;
 }

 /* int read_nonexistent_file_test()
 * Description: Test if the file system recognizes non-existing file correctly
 * Inputs: None
 * Outputs: Test result. Pass/ Fail
 * Returned Value: PASS upon success	
 * Side Effect:  None
 * Expected outcome: Pass
 */
 int read_nonexistent_file_test() {
	 dentry_t ref; /* Temporary variable to store file information (shall not contain a dentry in this case) */
	 if (read_dentry_by_name("ece391ishard.txt", &ref) == FS_SUCCESS) { /* Input filename is invalid */
		 return FAIL;
	 }
	 return PASS;
 }

/* int read_directory_by_valid_index_test()
 * Description: Test if the file system read by valid index works properly
 * Inputs: None
 * Outputs: Test result. Pass/ Fail
 * Returned Value: PASS upon success	
 * Side Effect:  None
 * Expected outcome: Pass
 */ 
 int read_directory_by_valid_index_test() {
	 dentry_t ref; /* Temporary variable to store file information. */
	 int32_t read_result; /* Flag to see if read succeeded */
	 uint32_t cur_type; /* Type of current file */
	 uint32_t idx; /* Index */
	 idx = 0; /* Initialize it to represent the index of the directory itself */
	 read_result = read_dentry_by_index(idx, &ref); /* Attempt to read */
	 if (read_result == FS_FAILURE) {
		 return FAIL;
	 }
	 cur_type = ref.file_type; /* Get type of current file */
	 if (cur_type != FOLDER_TYPE_FILE) { /* Check if type of "directory" itself is folder */
		return FAIL;
	 }
	 if (ref.inode_num != 0) { /* Directory entry for folder shouldn't have an inode number */
		return FAIL;
	 }
	 return PASS;
 }

 /* int read_directory_by_invalid_index_test()
 * Description: Test if the file system recognizes invalid reading index
 * Inputs: None
 * Outputs: Test result. Pass/ Fail
 * Returned Value: PASS upon success	
 * Side Effect:  None
 * Expected outcome: Pass
 */
 int read_directory_by_invalid_index_test() {
	 dentry_t ref; /* Temporary variable to store file information (shall not contain a dentry in this case) */
	 if (read_dentry_by_index(256, &ref) == FS_SUCCESS) { /* Try an index out of bounds(63) */
		 return FAIL;
	 }
	 return PASS;
 }

 /* int read_overlength_filename_test()
 * Description: Test if the file system recognizes overlength file name correctly
 * Inputs: None
 * Outputs: Test result. Pass/ Fail
 * Returned Value: PASS upon success	
 * Side Effect:  None
 * Expected outcome: Pass
 */ 
 int read_overlength_filename_test() {
	 dentry_t ref; /* Temporary variable to store file information (shall not contain a dentry in this case) */
	 int32_t read_result; /* Flag to see if read succeeds */
	 read_result = read_dentry_by_name("verylargetextwithverylongname.txt", &ref); /* File exists but full name overlength */
	 if (read_result == FS_SUCCESS) { /* Shall fail to read */
		return FAIL;
	 }
	 return PASS;
 }

/* int read_very_large_existent_file_test()
 * Description: Test if the file system reads a very large existent file correctly
 * Inputs: None
 * Outputs: Test result. Pass/ Fail
 * Returned Value: PASS upon success	
 * Side Effect:  None
 * Expected outcome: Pass
 */ 
 int read_very_large_existent_file_test() {
	 dentry_t ref; /* Temporary variable to store file information. */
	 char buf[33]; /* A buf with 33 bytes (including the ending byte */
	 if (read_dentry_by_name("verylargetextwithverylongname.tx", &ref) == FS_FAILURE) {
		 return FAIL; /* Fails if we cannot read the file */
	 }
	 if (fs_inode_length(ref.inode_num != 5277)) { /* See if the file we read is 5277 bytes long */
		 return FAIL;
	 }
	 buf[32] = '\0'; /* Last byte of buf signifies end of string */
	 char exp1[] = "very large text file with a very";
	 char exp2[] = "12345678901234567890123456789012";
	 char exp3[] = "0\n123456789012345678901234567890";
	 char exp4[] = "abcdefghijklmnopqrstuvwxyzabcdef";
	 char exp5[] = "<>?\n"; /* Initialize expected outputs */
	 /* Read first 32 bytes of the file*/
	 if (read_data(ref.inode_num, 0, buf, 32) != 32 || strncmp(buf, exp1, 32) != 0) { 
		 return FAIL;
	 }
	 /* Read first 32 bytes of line 2*/
	 if (read_data(ref.inode_num, 43, buf, 32) != 32 || strncmp(buf, exp2, 32) != 0) { 
		 return FAIL;
	 }
	 /* Read 32 bytes starting from the last byte of line 2*/
	 if (read_data(ref.inode_num, 122, buf, 32) != 32 || strncmp(buf, exp3, 32) != 0) { 
		 return FAIL;
	 }
	 /* Read 32 bytes in a random segment of the file*/
	 if (read_data(ref.inode_num, 4081, buf, 32) != 32 || strncmp(buf, exp4, 32) != 0) { 
		 return FAIL;
	 }
	 /* Read 4 bytes at the end of the file (length still set to 32 (try reading over length) to test truncation)*/
	 if (read_data(ref.inode_num, 5273, buf, 32) != 4 || strncmp(buf, exp5, 4) != 0) { 
		 return FAIL;
	 }
	 /* Test reading out of bounds - shall not read any data*/
	 if (read_data(ref.inode_num, 5999, buf, 32) != 0) { 
		 return FAIL;
	 }
	 return PASS;
 }

/* int list_directory_test()
 * Description: List the directories in file system, calling read_dir
 * Inputs: None
 * Outputs: Test result. Pass/ Fail
 * Returned Value: PASS upon success	
 * Side Effect:  None
 * Expected outcome: Pass
 */ 
 int list_directory_test() {
	 int32_t num_bytes_read; /* # bytes read returned by read_dir() */
	 int loop_idx; /* Loop index */
	 char buf[1 + MAX_FILENAME_LENGTH]; /* The buf we write to */
	 for (loop_idx = 0; loop_idx < MAX_FILE_NUM; loop_idx++) {
		 num_bytes_read = read_dir(loop_idx, buf, MAX_FILENAME_LENGTH);
		 if (num_bytes_read == FS_FAILURE || num_bytes_read > MAX_FILENAME_LENGTH) { /* Check if success */
			 return FAIL;
		 }
		 if (num_bytes_read == 0) {
			 break; /* Reaches end */
		 }
		 buf[num_bytes_read] = '\0'; /* Signifies end of string */
		 printf("file_name: ");
		 printf(buf); /* Print thr filename */
		 printf("\n"); /* Prepare to print next file on a new line */
	 }
	 return PASS;
 }

/* int interface_read_existent_file_test()
 * Description: Use file_open, file_read and file_close to read existent file
 * Inputs: None
 * Outputs: Test result. Pass/ Fail
 * Returned Value: PASS upon success	
 * Side Effect:  None
 * Expected outcome: Pass
 */ 
 int interface_read_existent_file_test() {
	 int32_t ref_fd; /* Referenced file directory */
	 uint32_t offset; /* Read offset */
	 char buf[256]; /* A buf with greater size than that of file */
	 int loop_idx; /* Loop index */
	 offset = 0; /* Offset initialized at 0 */
	 if (file_open(&ref_fd, "frame1.txt") == FS_FAILURE) { /* Opens the file and check if succeeded */
		 return FAIL;
	 }
	 if (file_read(&ref_fd, &offset, buf, 256) == FS_FAILURE) { /* Read the file and check if succeeded */
		 return FAIL;
	 }
	 if (offset != 174) { /* check file length */
		 return FAIL;
	 }
	 for (loop_idx = 0; loop_idx < offset; loop_idx++) {
		 putc(buf[loop_idx], visible_terminal); /* Print the file */
	 }
	 if (file_close(&ref_fd) == FS_FAILURE) { /* Close the file and check if succeeded */
		 return FAIL;
	 }
	 if (ref_fd != 0) { /* Inode index isn't cleared */
		 return FAIL;
	 }
	 return PASS;
 }

 /* int interface_read_nonexistent_file_test()
 * Description: Recognizes nonexistent file
 * Inputs: None
 * Outputs: Test result. Pass/ Fail
 * Returned Value: PASS upon success	
 * Side Effect:  None
 * Expected outcome: Pass
 */ 
 int interface_read_nonexistent_file_test() {
	 int32_t ref_fd; /* Referenced file directory */
	 if (file_open(&ref_fd, "nonexistent.txt") == FS_SUCCESS) { /* Check if succeeded (it should not)*/
		 return FAIL;
	 }
	 if (ref_fd != 0) { /* Should not scramble fd */
		 return FAIL;
	 }
	 return PASS;
 }

 /* int ls_simulator_test()
 * Description: Simulates "ls" exe
 * Inputs: None
 * Outputs: Test result. Pass/ Fail
 * Returned Value: PASS upon success	
 * Side Effect:  None
 * Expected outcome: Pass
 */  
 int ls_simulator_test() {
	 int32_t ref_fd = 0; /* Referenced file directory */
	 char buf[1 + MAX_FILENAME_LENGTH]; /* Buf to putput file name */
	 int32_t returned_length; /* Returned file name length from dir_read() */
	 uint32_t offset; /* Offset */
	 if (dir_open(&ref_fd, ".") == FS_FAILURE) { /* Try to open the only directory */
		return FAIL;
	 }
	 offset = 0; /* Offset initialized at 0 */
	 while(1) { /* Iterating until break */
	 	 buf[0] = '\0'; /* Reset */
		 returned_length = dir_read(&ref_fd, &offset, buf, MAX_FILENAME_LENGTH); /* Read current entry */
		 if (returned_length == -1 || returned_length > MAX_FILENAME_LENGTH) { /* Check validty */
			 return FAIL;
		 }
		 if (returned_length == 0) { /* Reaching the end */
			 break;
		 }
		 buf[returned_length] = '\0'; /* Signifies end of string */
		 printf("file_name: ");
		 printf(buf); /* Print the file name */
		 putc('\n', visible_terminal); /* Go to next line */
	 }
	 if (dir_close(&ref_fd) == FS_FAILURE) { /* Close the directory and check if succeeded */
		 return FAIL;
	 }
	 if (ref_fd != 0) { /* Check if fd isn't cleared */
		 return FAIL;
	 }
	 return PASS;
 }
/* Checkpoint 3 tests */
int syscall_execute_test() {
	char* command = "testprint";
	printf("command is %s\n", command);
	execute ((uint8_t*)command);
	printf("success");
	return PASS;
}

int test_garbage_read() {
	printf("testing garbage read\n");
	terminal_read(NULL, 0, 0, 0);
	//file_read(NULL, 0,0,0);
	printf("success\n");
	return PASS;
}

int test_garbage_write() {
	printf("testing garbage write\n");
	terminal_write(NULL, 0, 0, 0);
	rtc_write(NULL, 0, 0, 0);
	file_write(NULL, 0,0,0);
	printf("success\n");
	return PASS;
}

int test_garbage_open() {
	printf("testing garbage open\n");
	//file_open(NULL, 0);
	printf("success\n");
	return PASS;
}
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	clear();
	reset_cursor();

	while(1) {
		terminal_test();
	}

	//test_garbage_read();
	//test_garbage_write();
	//test_garbage_open();

	printf("\n\npress enter to continue");
	wait_for_enter();

	clear();
	reset_cursor();
	/**
	printf("start file system test\n\n");
	TEST_OUTPUT("File Sys Loaded Test", initialization_test());
	TEST_OUTPUT("Read Nonexist Text File Test", read_nonexistent_file_test())
	TEST_OUTPUT("Read Directory By Valid Index Test", read_directory_by_valid_index_test());
	printf("\n\npress enter to continue");
	wait_for_enter();

	clear();
	reset_cursor();
	TEST_OUTPUT("Read Directory By Invalid Index Test", read_directory_by_invalid_index_test());
	TEST_OUTPUT("Read Overlength Filename Test", read_overlength_filename_test());
	TEST_OUTPUT("List Directory Test", list_directory_test());
	
	printf("\n\npress enter to continue");
	wait_for_enter();

	clear();
	reset_cursor();
	TEST_OUTPUT("Interface Read Existent File Test", interface_read_existent_file_test());
	//TEST_OUTPUT("Read Existent Text File Test", read_existent_file_test_1()); // Test to read short txt
	printf("\n\npress enter to continue");
	wait_for_enter();

	clear();
	reset_cursor();
	TEST_OUTPUT("Read Existent EXE File Test", read_existent_file_test_2()); // Test to read executable / large file
	printf("\n\npress enter to continue");
	wait_for_enter();

	clear();
	reset_cursor();
	TEST_OUTPUT("Read Large File", read_very_large_existent_file_test()); // Test to verify that file reading is correct
	printf("\n\npress enter to continue");
	wait_for_enter();

	clear();
	reset_cursor();
	TEST_OUTPUT("LS SIMULATOR TEST", ls_simulator_test()); // Test for "ls"
	printf("\n\npress enter to continue");
	wait_for_enter();

	clear();
	reset_cursor();
	rtc_test();

	printf("\n\nRTC test completed, press enter to continue");
	wait_for_enter();
	
	clear();
	reset_cursor();
	printf("start terminal test\n");
	while(1) {
		terminal_test();
	}
	*/


}
