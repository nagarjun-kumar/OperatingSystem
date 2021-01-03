#include "keyboard.h"

//scancode_t has 2 attributes: key name and scancode value
typedef struct scancode {
    uint8_t key;
    uint8_t value;
} scancode_t;

//ascii_t has 2 attributes: key name and ascii value
typedef struct ascii {
    uint8_t key;
    uint8_t value;
} ascii_t;

uint8_t scancode_to_key(scancode_t* map, uint8_t scan);
uint8_t key_to_ascii(ascii_t* map, uint8_t key);
void echo_key(uint8_t key);

//declare all the key names we use
enum uint8_t {
    ESCAPE, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, ZERO_, DASH, EQUAL, BACKSPACE,

    TAB, Q, W, E, R, T, Y, U, I, O, P, LEFT_BRACKET, RIGHT_BRACKET, ENTER,

    LEFT_CONTROL, A, S, D, F, G, H, J, K, L, SEMICOLON, SINGLE_QUOTE, BACKTICK, LEFT_SHIFT, BACKSLASH,

    Z, X, C, V, B, N, M, COMMA, PERIOD, SLASH, RIGHT_SHIFT, KEYPAD,

    LEFT_ALT, SPACE, CAPS_LOCK, 
    CAPS_LOCK_RELEASED, LEFT_CONTROL_RELEASED, LEFT_SHIFT_RELEASED, RIGHT_SHIFT_RELEASED, UP, DOWN,
    LEFT_ALT_RELEASED,

    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,

    UNKOWN_KEY
};

/* scancide table defined below. This scancode table will map each scancode to key name declared above*/
static scancode_t scancode_table[SCANCODE_SIZE] = {
    {ESCAPE, 0x01}, {ONE, 0x02}, {TWO, 0x03}, {THREE, 0x04}, {FOUR, 0x05}, {FIVE, 0x06}, {SIX, 0x07},
    {SEVEN, 0x08}, {EIGHT, 0x09}, {NINE, 0x0A}, {ZERO_, 0x0B}, 

    {DASH, 0x0C}, {EQUAL, 0x0D}, {BACKSPACE, 0x0E}, {TAB, 0x0F}, 

    {Q, 0x10}, {W, 0x11}, {E, 0x12}, {R, 0x13}, {T, 0x14}, {Y, 0x15}, {U, 0x16}, {I, 0x17}, {O, 0x18}, {P, 0x19}, 
    {LEFT_BRACKET, 0x1A}, {RIGHT_BRACKET, 0x1B}, {ENTER, 0x1C},

    {LEFT_CONTROL, 0x1D}, {A, 0x1E}, {S, 0x1F}, {D, 0x20}, {F, 0x21}, {G, 0x22}, {H, 0x23}, {J, 0x24}, {K, 0x25}, {L, 0x26}, 
    {SEMICOLON, 0x27}, {SINGLE_QUOTE, 0x28}, {BACKTICK, 0x29}, {LEFT_SHIFT, 0x2A}, {BACKSLASH, 0x2B},

    {Z, 0x2C}, {X, 0x2D}, {C, 0x2E}, {V, 0x2F}, {B, 0x30}, {N, 0x31}, {M, 0x32}, 
    {COMMA, 0x33}, {PERIOD, 0x34}, {SLASH, 0x35}, {RIGHT_SHIFT, 0x36}, {KEYPAD, 0x37},

    {LEFT_ALT, 0x38}, {SPACE, 0x39}, {CAPS_LOCK, 0x3A}, {UP, 0x48}, {DOWN, 0x50},
    {CAPS_LOCK_RELEASED, 0xBA}, {LEFT_CONTROL_RELEASED, 0x9D}, {LEFT_SHIFT_RELEASED, 0xAA}, {RIGHT_SHIFT_RELEASED, 0xB6},
    {LEFT_ALT_RELEASED, 0xB8},

    {F1, 0x3B}, {F2, 0x3C}, {F3, 0x3D}, {F4, 0x3E}, {F5, 0x3F}, {F6, 0x40}, {F7, 0x41}, {F8, 0x42}, {F9, 0x43}, {F10, 0x44} 
};

/* ascii table defined below. This ascii table will map key name declared above to corresponding ascii value*/
static ascii_t ascii_table[ASCII_SIZE] = {
    {ZERO_, 0x30}, {ONE, 0x31}, {TWO, 0x32}, {THREE, 0x33}, {FOUR, 0x34}, {FIVE, 0x35}, {SIX, 0x36}, {SEVEN, 0x37}, {EIGHT, 0x38}, {NINE, 0x39},

    {A, 0x61}, {B, 0x62}, {C, 0x63}, {D, 0x64}, {E, 0x65}, {F, 0x66}, {G, 0x67}, {H, 0x68}, {I, 0x69}, 
    {J, 0x6A}, {K, 0x6B}, {L, 0x6C}, {M, 0x6D}, {N, 0x6E}, {O, 0x6F}, {P, 0x70}, {Q, 0x71}, {R, 0x72}, 
    {S, 0x73}, {T, 0x74}, {U, 0x75}, {V, 0x76}, {W, 0x77}, {X, 0x78}, {Y, 0x79}, {Z, 0x7A},
    {DASH, 0x2D}, {EQUAL, 0x3D}, {LEFT_BRACKET, 0x5B}, {RIGHT_BRACKET, 0x5D}, {BACKSLASH, 0x5C}, {SEMICOLON, 0x3B},
	{SINGLE_QUOTE, 0x27}, {BACKTICK, 0x60}, {COMMA, 0x2C}, {PERIOD, 0x2E}, {SLASH, 0x2F}, {SPACE, 0x20}

};

/* ascii table defined below. this table maps keys when they are pressed with shift.
*/
static ascii_t shift_ascii_table[ASCII_SIZE] = {
	{ZERO_, 0x29}, {ONE, 0x21}, {TWO, 0x40}, {THREE, 0x23}, {FOUR, 0x24}, {FIVE, 0x25}, {SIX, 0x5E}, {SEVEN, 0x26}, {EIGHT, 0x2A}, {NINE, 0x28},
	{A, 0x41}, {B, 0x42}, {C, 0x43}, {D, 0x44}, {E, 0x45}, {F, 0x46}, {G, 0x47}, {H, 0x48}, {I, 0x49},
	{J, 0x4A}, {K, 0x4B}, {L, 0x4C}, {M, 0x4D}, {N, 0x4E}, {O, 0x4F}, {P, 0x50}, {Q, 0x51}, {R, 0x52},
	{S, 0x53}, {T, 0x54}, {U, 0x55}, {V, 0x56}, {W, 0x57}, {X, 0x58}, {Y, 0x59}, {Z, 0x5A},
	{DASH, 0x5F}, {EQUAL, 0x2B}, {LEFT_BRACKET, 0x7B}, {RIGHT_BRACKET, 0x7D}, {BACKSLASH, 0x7C}, {SEMICOLON, 0x3A},
	{SINGLE_QUOTE, 0x22}, {BACKTICK, 0x7E}, {COMMA, 0x3C}, {PERIOD, 0x3E}, {SLASH, 0x3F}, {SPACE, 0x20}
};

/*ascii table defined below. this table maps keys when caps is pressed first.
*/
static ascii_t caps_ascii_table[ASCII_SIZE] = {
	{ZERO_, 0x30}, {ONE, 0x31}, {TWO, 0x32}, {THREE, 0x33}, {FOUR, 0x34}, {FIVE, 0x35}, {SIX, 0x36}, {SEVEN, 0x37}, {EIGHT, 0x38}, {NINE, 0x39},
	{A, 0x41}, {B, 0x42}, {C, 0x43}, {D, 0x44}, {E, 0x45}, {F, 0x46}, {G, 0x47}, {H, 0x48}, {I, 0x49},
	{J, 0x4A}, {K, 0x4B}, {L, 0x4C}, {M, 0x4D}, {N, 0x4E}, {O, 0x4F}, {P, 0x50}, {Q, 0x51}, {R, 0x52},
	{S, 0x53}, {T, 0x54}, {U, 0x55}, {V, 0x56}, {W, 0x57}, {X, 0x58}, {Y, 0x59}, {Z, 0x5A},
	{DASH, 0x2D}, {EQUAL, 0x3D}, {LEFT_BRACKET, 0x5B}, {RIGHT_BRACKET, 0x5D}, {BACKSLASH, 0x5C}, {SEMICOLON, 0x3B},
	{SINGLE_QUOTE, 0x27}, {BACKTICK, 0x60}, {COMMA, 0x2C}, {PERIOD, 0x2E}, {SLASH, 0x2F}, {SPACE, 0x20}
};

/*ascii table defined below, this table maps keys when caps is pressed first and they are pressed with shift.
*/
static ascii_t caps_shift_ascii_table[ASCII_SIZE] = {
	{ZERO_, 0x29}, {ONE, 0x21}, {TWO, 0x40}, {THREE, 0x23}, {FOUR, 0x24}, {FIVE, 0x25}, {SIX, 0x5E}, {SEVEN, 0x26}, {EIGHT, 0x2A}, {NINE, 0x28},
	{A, 0x61}, {B, 0x62}, {C, 0x63}, {D, 0x64}, {E, 0x65}, {F, 0x66}, {G, 0x67}, {H, 0x68}, {I, 0x69}, 
    {J, 0x6A}, {K, 0x6B}, {L, 0x6C}, {M, 0x6D}, {N, 0x6E}, {O, 0x6F}, {P, 0x70}, {Q, 0x71}, {R, 0x72}, 
    {S, 0x73}, {T, 0x74}, {U, 0x75}, {V, 0x76}, {W, 0x77}, {X, 0x78}, {Y, 0x79}, {Z, 0x7A}, 
	{DASH, 0x5F}, {EQUAL, 0x2B}, {LEFT_BRACKET, 0x7B}, {RIGHT_BRACKET, 0x7D}, {BACKSLASH, 0x7C}, {SEMICOLON, 0x3A},
	{SINGLE_QUOTE, 0x22}, {BACKTICK, 0x7E}, {COMMA, 0x3C}, {PERIOD, 0x3E}, {SLASH, 0x3F}, {SPACE, 0x20}
};


/*
 * keyboard_init
 * DESCRIPTION: keyboard initialization function. Initialize keyboard by unmasking the PIC
 * INPUT: NONE
 * OUTPUT: NONE
 */
void keyboard_init(void){
    caps_flag = 0;
    shift_flag = 0;
    ctrl_flag = 0;
    alt_flag = 0;
    enable_irq(KEYBOARD_IRQ);
}

/*
 * scancode_to_key
 * DESCRIPTION: this functionwill take scancode table and scancode value, and return the corresponding key name
 * INPUT: map: scancode map
 *        scan: scancode of the key pressed
 * OUTPUT: key name of the key pressed
 */
uint8_t scancode_to_key(scancode_t* map, uint8_t scan) {
    uint8_t toReturn = UNKOWN_KEY;

    int i;          //loop index
    for (i = 0; i < SCANCODE_SIZE; i++) {
        if (map[i].value == scan) {
            toReturn = map[i].key;
        }
    }
    return toReturn;
}
/*
 * key_to_ascii
 * DESCRIPTION: this function will take ascii table and key name, and return corresponding ascii value
 * INPUT: map: ascii map
 *        key: key name of the key pressed
 * OUTPUT: ascii value of the key pressed
 */ 
uint8_t key_to_ascii(ascii_t* map, uint8_t key) {
    uint8_t toReturn = 0;
    int i = 0;               //loop index
    for (i = 0; i < ASCII_SIZE; i++) {
        if (map[i].key == key) {
            toReturn = map[i].value;
        }
    }
    return toReturn;
}

/* void clear_kayboard_buffer(void)
* description: clear the keyboard buffer
* input: void
* outuput: none
* return value: none
*/
void clear_kayboard_buffer(int32_t terminal_id) {
    int i; //loop index
    //clear the keyboard buffer
    for(i = 0; i <MAX_BUF; i++) {
        term[terminal_id].line_buff[i] = NULL;
    }
    term[terminal_id].line_buff[0] = NEW_LINE;
    //reset buffer index to 0
    term[terminal_id].buf_idx = 0;
}

/*
 * echo_key
 * DESCRIPTION: This function will show the key being typed to desktop
 * INPUT: key: key name of the key being pressed
 * OUTPUT: NONE
 * SIDE EFFECT: prrint the key being pressed to screen
 */
void echo_key(uint8_t key) {
    uint8_t toReturn;
    if (!shift_flag && !caps_flag) {
        toReturn = key_to_ascii(ascii_table, key);
    } else if(shift_flag && !caps_flag) {
        toReturn = key_to_ascii(shift_ascii_table, key);
    } else if(!shift_flag && caps_flag) {
        toReturn = key_to_ascii(caps_ascii_table, key);
    } else if(shift_flag && caps_flag) {
        toReturn = key_to_ascii(caps_shift_ascii_table, key);
    }
    
    //check null condition
    if (toReturn == 0) {
        return;
    }

    if (term[visible_terminal].buf_idx < MAX_BUF - 1) {
        term[visible_terminal].line_buff[term[visible_terminal].buf_idx] = toReturn;
        term[visible_terminal].line_buff[term[visible_terminal].buf_idx + 1] = NEW_LINE;
        term[visible_terminal].buf_idx++;

    } else if (term[visible_terminal].buf_idx == MAX_BUF - 1) {
        term[visible_terminal].line_buff[buf_idx] = NEW_LINE;

    } else {
        term[visible_terminal].buf_idx = 0;
    }

    putc(toReturn, visible_terminal);
    return;
}

/*
 * KEYBOARD_INTERRUPT
 * DESCRIPTION: keyboard interrupt handler
 * INPUT: NONE
 * OUTPUT: NONE
 * SIDE EFFECT: see which key is being pressed and if the pressed key is a recognizable key,
 *              print it to the screen
 */
void keyboard_handler(void) {
    uint8_t scan = inb(KEYBOARD_PORT);
    uint8_t key = scancode_to_key(scancode_table, scan);
    uint32_t vm_idx; /* Video memory index in page tables */

    //If the key is special key, we first handle those
    if (key == CAPS_LOCK) {     // deal caps lock key
        if (caps_flag == 0) {
            caps_flag = 1;
        } else if (caps_flag == 1) {
            caps_flag = 0;
        }
        send_eoi(KEYBOARD_IRQ);
        return;
    } else if(key == CAPS_LOCK_RELEASED) {  //when caps lock released, do nothing
        send_eoi(KEYBOARD_IRQ);
        return;
    } else if (key == LEFT_CONTROL) {   //set ctrl flag on when ctrl pressed
        ctrl_flag = 1;
        send_eoi(KEYBOARD_IRQ);
        return;
    } else if(key == LEFT_CONTROL_RELEASED) {   //clear ctrl flag when ctrl released
        ctrl_flag = 0;
        send_eoi(KEYBOARD_IRQ);
        return;
    } else if (key == LEFT_SHIFT || key == RIGHT_SHIFT) {   //when shift pressed, set shift flag
        shift_flag = 1;
        send_eoi(KEYBOARD_IRQ);
        return;
    } else if (key == LEFT_SHIFT_RELEASED || key == RIGHT_SHIFT_RELEASED) { //cleae shift flag when shift released
        shift_flag = 0;
        send_eoi(KEYBOARD_IRQ);
        return;
    } else if (key == TAB) {
        echo_key(SPACE);
        send_eoi(KEYBOARD_IRQ);
        return;
    } else if (key == LEFT_ALT) {
        alt_flag = 1;
    } else if (key == LEFT_ALT_RELEASED) {
        alt_flag = 0;
    } 
    
    if (alt_flag == 1 && key == F1) {
        switch_terminal(0);
    } else if (alt_flag == 1 && key == F2) {
        switch_terminal(1);
    } else if (alt_flag == 1 && key == F3) {
        switch_terminal(2);
    }

    vm_idx = ((VID_MEM_ADD & MASK_21_12) >> TBL_OFFSET);
    page_table[vm_idx].page_start_add = vm_idx; /* Put it on vid. mem. */
    asm volatile ( /* Flush the TLB by writing to register cr3 */
      "movl %%cr3, %%eax     ;"
      "movl %%eax, %%cr3     ;"
      : /* No outputs */
      : /* No inputs */
      : "eax", "cc"  /* Clobbers */
      );
    
    if (key == BACKSPACE) {  //when backspace pressed, delete previous character if it exists
        
        if (term[visible_terminal].buf_idx > 0) {      //also delete that character from keyboard buffer
            backspace();
            term[visible_terminal].line_buff[term[visible_terminal].buf_idx] = NULL;
            term[visible_terminal].line_buff[term[visible_terminal].buf_idx - 1] = NEW_LINE;
            term[visible_terminal].buf_idx--;
        }

        send_eoi(KEYBOARD_IRQ);
    }

    if (ctrl_flag == 1 && key == L) {    //when ctrl + L or ctrl + l pressed, clear screen
        clear();
        reset_cursor();
        clear_kayboard_buffer(visible_terminal);
        if (term[visible_terminal].pcb->shell_flag) {
            //terminal_write(0,0,"391OS> ",7);
            printf("391OS> ");
        }
        if (visible_terminal == running_terminal) {/* Process is on screen */
            page_table[vm_idx].page_start_add = vm_idx;
        } else { /* Process not on screen */
            page_table[vm_idx].page_start_add = vm_idx + 1 + running_terminal;
        }
        asm volatile ( /* Flush the TLB by writing to register cr3 */
        "movl %%cr3, %%eax     ;"
        "movl %%eax, %%cr3     ;"
        : /* No outputs */
        : /* No inputs */
        : "eax", "cc"  /* Clobbers */
        );
        send_eoi(KEYBOARD_IRQ);
        return;
    } 

    if (key == ENTER) {  //when enter pressed, go to next line
        putc('\n', visible_terminal);
        term[visible_terminal].enter_flag = 1;
        send_eoi(KEYBOARD_IRQ);
        if (visible_terminal == running_terminal) {/* Process is on screen */
            page_table[vm_idx].page_start_add = vm_idx;
        } else { /* Process not on screen */
            page_table[vm_idx].page_start_add = vm_idx + 1 + running_terminal;
        }
        asm volatile ( /* Flush the TLB by writing to register cr3 */
        "movl %%cr3, %%eax     ;"
        "movl %%eax, %%cr3     ;"
        : /* No outputs */
        : /* No inputs */
        : "eax", "cc"  /* Clobbers */
        );
        return;
    }

    //check if the pressed key is printable
    if (key != UNKOWN_KEY) {    
        echo_key(key);
    }

    if (visible_terminal == running_terminal) {/* Process is on screen */
        page_table[vm_idx].page_start_add = vm_idx;
    } else { /* Process not on screen */
        page_table[vm_idx].page_start_add = vm_idx + 1 + running_terminal;
    }
    asm volatile ( /* Flush the TLB by writing to register cr3 */
      "movl %%cr3, %%eax     ;"
      "movl %%eax, %%cr3     ;"
      : /* No outputs */
      : /* No inputs */
      : "eax", "cc"  /* Clobbers */
      );
    //send eoi signal to PIC
    send_eoi(KEYBOARD_IRQ);
    return;
}


