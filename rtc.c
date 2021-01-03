#include "rtc.h"
#include "lib.h"

/* Jump table for fs abstraction when dealing rtc */
fs_jump_table_t rtc_jmptable = {
    .open = rtc_open,
    .close = rtc_close,
    .read = rtc_read,
    .write = rtc_write
};

/*
 * rtc_init()
 * Description: Initialize RTC and turn on real time clock interrupt 
 *              For detail, refer to https://wiki.osdev.org/RTC
 * INPUTS: NONE
 * OUTPUTS: NONE
 */
void rtc_init() {
    char prev;
    rtc_interrupt_occured = 0;              //set interrupt occured to 0, this means no interrupt occured 
    outb(REGISTER_B, RTC_PORT);               //select register B, and disable NMI
    prev = inb(CMOS_PORT);               //read the current value of register B
    outb(REGISTER_B, RTC_PORT);               //set the index again (a read will reset the index to register D)
    outb(prev | BIT_SIX , CMOS_PORT);       //write the previous value ORed with 0x40. This turns on bit 6 of register B

    enable_irq(RTC_IRQ);                  //enable interrupt
}

/*
 * RTC_INTERRUPT()
 * DESCRIPTION: RTC interrupt handler
 * INPUT: NONE
 * OUTPUT: NONE
 */
void rtc_handler(void) {
    outb(REGISTER_C_NMI, RTC_PORT);
    inb(CMOS_PORT);
    //set interrupt occured flag. record that interrupt happened
    rtc_interrupt_occured = 1;
    return;
}


/* int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes)
* description: read from rtc register and process the datas.
* input: fd, buf, nbytes
* output: none
* return value: 0 when success
* side effect: wait for interrupt change to one
*/
int32_t rtc_read(int32_t* fd, uint32_t* offset,  char* buf, uint32_t nbytes) {

    rtc_interrupt_occured = 0;  //set flag to 0, this means no interrupt occur
    //use while loop to wait until interrupt handler sets the flag
    sti();
    while (!rtc_interrupt_occured){}
    cli();
    rtc_interrupt_occured = 0;  //clear the flag back to 0
    return 0;
}

/* int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes)
* description: this function writes a new frequency betwen 2 and 1024
* input: fd, buf, nbytes
* output: none
* return value: 0 for success, -1 for failure
* side effect: set new frequency of RTC
*/
int32_t rtc_write(int32_t* fd, uint32_t* offset, const char* buf, uint32_t nbytes) {
    int freq;       //frequency value passed by buf argument
    int freq_check;     //flag used to check power of 2
    int i;          //loop index
    int rate;       //rate of RTC, freq = 32768 >> (rate - 1)
    char prev;

    uint8_t mask_low = 0x01;
    //if buf is null, return -1
    if (buf == NULL) {
        return -1;
    }
    //if size if not 4, return -1
    if (nbytes != BYTE) {
        return -1;
    }
    freq = *((int*)buf);
    //if frequency is lower than 2 or greater than 1024, return -1
    if (freq < MIN_FREQ || freq > MAX_FREQ) {
        return -1;
    }
    //following logic check if frequency is power of 2 or not
    freq_check = freq & (freq - 1);
    if (freq_check != 0) {
        return -1;
    }

    //we need to change freq to rate.
    //since freq is power of 2, we first need to find out log base 2 of freq
    rate = 0;
    for (i = 0; i < 10; i++) {  //since max is 1024, i can never be greater than 11
        if ((freq & mask_low) == 1) {
            break;
        }
        rate++;
        freq = freq >> 1;
    }
    rate = 16 - rate;       //get rate for RTC

    rate &= LOW_BYTE;			// rate must be above 2 and not over 15
    cli();
    outb(REGISTER_A, RTC_PORT);		// set index to register A, disable NMI
    prev=inb(CMOS_PORT);	// get initial value of register A
    outb(REGISTER_A, RTC_PORT);		// reset index to A
    outb((prev & HIGH_BYTE) | rate, CMOS_PORT); //write only our rate to A. Note, rate is the bottom 4 bits.

    return BYTE;
}

/*int32_t rtc_open(const uint8_t* filename)
* description: open the RTC
* input: filename
* output: none
* return value: 0 for success
*/
int32_t rtc_open(int32_t* fd, char* filename) {
    int rate, prev;

    rate = MAX_RATE;
    rate &= LOW_BYTE;			// rate must be above 2 and not over 15
    cli();
    outb(REGISTER_A, RTC_PORT);		// set index to register A, disable NMI
    prev=inb(CMOS_PORT);	// get initial value of register A
    outb(REGISTER_A, RTC_PORT);		// reset index to A
    outb((prev & HIGH_BYTE) | rate, CMOS_PORT); //write only our rate to A. Note, rate is the bottom 4 bits.

    return 0;
}

/*int32_t rtc_close(int32_t fd)
* description: close the RTC
* input: fd
* output: none
* return value: 0 for success
*/
int32_t rtc_close(int32_t* fd) {
    return 0;
}
