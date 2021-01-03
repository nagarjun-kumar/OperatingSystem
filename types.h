/* types.h - Defines to use the familiar explicitly-sized types in this
 * OS (uint32_t, int8_t, etc.).  This is necessary because we don't want
 * to include <stdint.h> when building this OS
 * vim:ts=4 noexpandtab
 */

#ifndef _TYPES_H
#define _TYPES_H

#define NULL 0
#define TASK_MAX_FILES 8  /* Each task can have up to 8 open files. */
#define MAX_ARG_LENGTH 128 /* Maximum length of argument in command is 128 characters */
#define MAX_BUF 128

#ifndef ASM

/* Types defined here just like in <stdint.h> */
typedef int int32_t;
typedef unsigned int uint32_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef char int8_t;
typedef unsigned char uint8_t;


/*------------------File Operations jump table--------------------------*/
typedef struct {
    /* Tasks in the jump table are type-specific */
    int32_t (*open)(int32_t*, char*);
    int32_t (*read)(int32_t*, uint32_t*, char*, uint32_t);
    int32_t (*write)(int32_t*, uint32_t*, const char*, uint32_t);
    int32_t (*close)(int32_t*);
} fs_jump_table_t;

/*--------------------Structure stored in the file array----------------*/
typedef struct {
    fs_jump_table_t* jmp_table; /* File operations table pointer */
    int32_t inode; /* The inode number for this file */
    uint32_t file_position; /* keeps track of where the user is currently reading from in the file */
    uint32_t flags; /* Marking this file desriptor as in-use. */
} file_arr_struct_t;

/*-----------------The Process Control Block----------------------------*/
typedef struct process_control_block{
    file_arr_struct_t file_desc_array[TASK_MAX_FILES]; /* The file descriptor array*/
    uint32_t cur_esp; /* Current esp */
    uint32_t cur_ebp; /* Current ebp */
    uint32_t cur_pid; /* Current pid */
    uint32_t parent_pid;  /* The process id of the parent task */
    uint32_t parent_esp; /* Save parent esp */
    uint32_t parent_ebp; /* Save parent ebp */
    uint32_t terminal_id; /* The terminal the process is running on */
    uint32_t override_flag;
    char arg[MAX_ARG_LENGTH + 1]; /* Save current argument */
    uint32_t existent; /* Signify if this pcb is existent */
    uint32_t shell_flag;
} pcb_t;

/*---------------------------terminal structure-------------------------*/
typedef struct {
    pcb_t * pcb; /* Active PCB */
    int32_t running_process; /* PID of running process */
    uint8_t id_terminal;
    uint8_t check_run;
    uint32_t term_x;
    uint32_t term_y;
    volatile uint8_t enter_flag;
    volatile uint8_t line_buff[MAX_BUF];
    volatile int32_t buf_idx;

    volatile uint8_t flag;
    int32_t vid_mem;


}term_t;

volatile uint32_t running_process_id; /* Pid of current running process */

#endif /* ASM */

#endif /* _TYPES_H */
