#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "i8259.h"
#include "types.h"
#include "lib.h"
#include "syscall.h"
#include "terminal.h"

#define KEYBOARD_PORT 0x60
#define KEYBOARD_IRQ 1
#define SCANCODE_SIZE 75
#define ASCII_SIZE 48
#define MAX_BUF 128
#define NEW_LINE 0x0A

/* keyboard initialization function */
extern void keyboard_init(void);
/* keyboard interrupt handler */
extern void keyboard_handler(void);
/* function use to clear keyboard buffer */
extern void clear_kayboard_buffer(int32_t terminal_id);

volatile uint8_t keyboard_buf[MAX_BUF];
volatile int32_t buf_idx;

uint8_t caps_flag;      //set when caps lock is on
uint8_t ctrl_flag;      //set when ctrl is pressed
uint8_t shift_flag;     //set when shift is pressed
//volatile uint8_t enter_flag;    //set when enter is pressed
uint8_t alt_flag;       //set alt when alt is pressed

#endif
