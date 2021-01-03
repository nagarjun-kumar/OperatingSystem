/*
 * Header File to Support System calls
 */

#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"
#include "lib.h"
#include "x86_desc.h"
#include "fs.h"
#include "fs_abstraction.h" /* Uses file sys abstraction */
#include "paging.h"
#include "rtc.h"
#include "keyboard.h"
#include "terminal.h"

#define SYSCALL_SUCCESS 0
#define SYSCALL_FAILURE -1
#define TRUE_ 1
#define FALSE_ 0
#define INVALID -1  /* Stands for invalid indices, pids etc */
#define KERNEL_START_ADD 0x400000 /* Kernel starts at 4 MB */
#define KERNEL_PAGE_SIZE 0x400000 /* A kernal page is 4 MB large */
#define FOUR_MB  0x400000 /* 4MB has 0x400000 bytes */
#define PCB_STACK_SIZE 0x2000 /* Each pcb owns a 8 KB stack */
#define FILE_HEADER_LENGTH 40 /* By document, file header is 40 bytes long */
#define EXE_HEADER_MAGIC_0 0x7F /* Executable file byte 0 magic # */
#define EXE_HEADER_MAGIC_1 0x45 /* Executable file byte 1 magic # */
#define EXE_HEADER_MAGIC_2 0x4C /* Executable file byte 2 magic # */
#define EXE_HEADER_MAGIC_3 0x46 /* Executable file byte 3 magic # */
#define DIR_OFFSET 22 /* Page directory address has offset 22 */
#define PROGRAM_IMG_ADDRESS 0x08048000 /* Program image address */
#define PROCESS_BASE_ADDRESS 2 /* Process page starts at 8MB (2 4MB pages) */
#define REF_EXE_FILE_LEN 0x2000 /* Referenced file length for reading to userspace */
#define FOUR_BYTES 0x4 /* 4 bytes used for calculating starting address of stacks */
#define USER_STACK_ADDRESS 0x08400000 /* Start of user stack */
#define ENTRY_PT_OFFSET 24 /* Entry point starting byte */
#define MAX_NUM_PROCESSES 6 /* Checkpoint 5 regulates that at most 6 programs running */
#define SYSCALL_TOO_MANY_PROCESSES 2 /* A random number between 1 and 255 */
#define INVALID_PID -1 /* -1 stands for an invalid allocated pid */
#define EXCEPTION_IDX 256 /*value of status when halted because of exception */



/* System calls open, close, read, write, execute, halt */
int32_t open (const uint8_t* filename);
int32_t close (int32_t fd);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t execute (const uint8_t* command);
int32_t halt (uint8_t status);

/* System calls getargs, vidmap */
int32_t getargs (uint8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen_start);

/* Helper Functions */
pcb_t* get_active_pcb();
pcb_t* get_pcb(int32_t pid);
int32_t get_avaialable_pid();
pcb_t* file_desc_array_init(pcb_t* cur_pcb);
void multiterminal_init();
int32_t execute_helper (const uint8_t* command);
int32_t halt_helper (uint8_t status);


uint8_t halt_flag;


#endif


