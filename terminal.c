

#include "terminal.h"
#include "keyboard.h"
#include "paging.h"

/* Jump table for terminal stdin */
fs_jump_table_t terminal_stdin_jmptable = {
    .open = terminal_open,
    .close = terminal_close,
    .read = terminal_read,
    .write = NULL
};
/* Jump table for terminal stdout */
fs_jump_table_t terminal_stdout_jmptable = {
    .open = terminal_open,
    .close = terminal_close,
    .read = NULL,
    .write = terminal_write
};

int terminal_id;


/*
 * init_terminal()
 * Descripion: Initialize the terminal driver
 * INPUT: none
 * OUTPUT: none
 */ 
void init_terminal(){

    int i;
    //int iter_buff;
    for (i = 0; i< TERM_NUM; i++){
        term[i].pcb = (pcb_t*)NULL;
        term[i].id_terminal = i;
        term[i].term_x = 0;
        term[i].term_y = 0;
        term[i].buf_idx = 0;
        term[i].enter_flag = 0;
    }
    term[0].vid_mem = TERM0_VIDMEM;
    term[1].vid_mem = TERM1_VIDMEM;
    term[2].vid_mem = TERM2_VIDMEM;

    running_process_id = INVALID_PID; /* No process is running upon start */
    visible_terminal = 0;

    return;
}

/*
 * int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes)
 * Descripion: read the letter displayed on terminal
 * INPUT: fd: not used
 *         buf: buffer to write the characters in
 *         nbytes: not used
 * OUTPUT: number of bytes written
 */ 
int32_t terminal_read(int32_t* fd, uint32_t* offset, char* buf, uint32_t nbytes) {
    int i; //loop index
    int32_t buff_size;      //temporaly variable to store buffer index, or size
    uint8_t* buf_;
    //null check
    if (buf == NULL) {
        return -1;
    }
    //if buffer size greater than maximum, return error
    if (term[visible_terminal].buf_idx > MAX_BUF) {
        return -1;
    }
    
    //store the buffer index
    buff_size = 0;
    //cast buf to uint_8
    buf_ = (uint8_t*) buf;
    //return only when enter key is pressed
    sti();
    while (term[running_terminal].enter_flag == 0) {}
    cli();
    //copy keyboard buffer to terminal line buffer
    for (i = 0; i < term[running_terminal].buf_idx + 1; i++) {
        buf_[i] = term[running_terminal].line_buff[i];
        buff_size++;
    }

    //clear enter flag
    term[running_terminal].enter_flag = 0;
    //clear the keyboard buffer and reset buffer index
    clear_kayboard_buffer(running_terminal);
    
    return buff_size;
}

/*
 * int32_t terminal_write(int32_t fd, void* buf, int32_t nbytes)
 * Descripion: write buffer onto screen
 * INPUT: fd: not used
 *         buf: buffer which stores characters to be displayed
 *         nbytes: number of bytes to be written
 * OUTPUT: number of bytes written
 */ 
int32_t terminal_write(int32_t* fd, uint32_t* offset, const char* buf, uint32_t nbytes) {
    int i;  //loop index
    uint8_t* buf_;
    if (buf == NULL) {
        return -1;
    }
    buf_ = (uint8_t*) buf;
    for(i = 0; i < nbytes; i++) {
        putc(buf_[i], running_terminal);
    }
    return nbytes;

}

/*
 * int32_t terminal_open(const uint8_t *filename)
 * Descripion: open terminal. reset cursor and clear screen and keyboard buffer
 * INPUT: filename: not used
 * OUTPUT: none
 */ 
int32_t terminal_open(int32_t* fd, char *filename) {
    clear();
    reset_cursor();
    clear_kayboard_buffer(running_terminal);
    return 0;
}

/*
 * int32_t terminal_close(int32_t fd)
 * Descripion: close terminal. reset cursor and clear screen and keyboard buffer
 * INPUT: filename: not used
 * OUTPUT: none
 */ 
int32_t terminal_close(int32_t* fd) {
    return -1; // should always fail
}

/*
 * switch erminal
 * Descripion: switch visible terminal
 * INPUT: terminal_id -> to which terminal we are swicthing to
 * OUTPUT: none
 */ 
void switch_terminal(int32_t terminal_id) {
    int32_t term_prev;
    uint32_t vm_idx; /* Video memory index in page tables */
    /* we check if terminal id is valid over here */
    if (terminal_id < 0) {
        return;
    } else if (terminal_id > 2) {
        return;
    }

    /*save current visible terminal id */
    term_prev = visible_terminal;
    /*if we are switching to current terminal, we dont need to do anything */
    if (terminal_id == term_prev){
        return;
    }

    /*change visible terminal id to the id we are going to change */
    visible_terminal = terminal_id;

    /*restoring the video memory address*/
    vm_idx = ((VID_MEM_ADD & MASK_21_12) >> TBL_OFFSET);
    page_table[vm_idx].page_start_add = vm_idx; /* Put it on vid. mem. */
    asm volatile ( /* Flush the TLB by writing to register cr3 */
      "movl %%cr3, %%eax     ;"
      "movl %%eax, %%cr3     ;"
      : /* No outputs */
      : /* No inputs */
      : "eax", "cc"  /* Clobbers */
      );

    /*perform switch by storing current video memory and writing new video memory*/
    memcpy((char*) term[term_prev].vid_mem, (const char*) VID_ADD, VID_MEM_SIZE);
    memcpy((char*) VID_ADD, (const char*) term[visible_terminal].vid_mem, VID_MEM_SIZE);
    /*update cursor in needed*/
    update_cursor();
}
