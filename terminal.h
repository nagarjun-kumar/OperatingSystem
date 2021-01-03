#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "syscall.h"
#include "keyboard.h"
#include "fs_abstraction.h" /* Support fs abstraction */


#define TERM_NUM 3
#define VID_ADD 0xB8000
#define VID_MEM_SIZE 4096
#define TERM0_VIDMEM 0xB9000
#define TERM1_VIDMEM 0xBA000
#define TERM2_VIDMEM 0xBB000


//visible terminal
int32_t visible_terminal;
//running terminal
int32_t running_terminal;

term_t term[TERM_NUM];

/* terminal initialization function */
void init_terminal();
//int32_t execute(const uint8_t* command);
/* Terminal read function */
int32_t terminal_read(int32_t* fd, uint32_t* offset, char* buf, uint32_t nbytes);
/* terminal write function */
int32_t terminal_write(int32_t* fd, uint32_t* offset, const char* buf, uint32_t nbytes);
/* terminal open function */
int32_t terminal_open(int32_t* fd, char *filename);
/*terminal close function */
int32_t terminal_close(int32_t* fd);

extern void switch_terminal(int32_t terminal_id);
//int32_t vidmap(uint8_t **screen_start);
//int32_t sigreturn (void);


/* Jump tables for fs abstraction */
extern fs_jump_table_t terminal_stdin_jmptable;
extern fs_jump_table_t terminal_stdout_jmptable;

#endif
