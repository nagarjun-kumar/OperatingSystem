#include "pit.h"
#include "lib.h"
#include "i8259.h"

void pit_init() {
    uint16_t divider;
    uint8_t lobyte;
    uint8_t hibyte;
    /*
    * we want interrupt every 10ms
    * therefore, we need 100hz frequency
    * since we know our base frequency, we need to find divider
    * since base_freq / divider = frequency, we need to do base_freq / freq to get divider
    * PIT only accepts 8 bit value, we divide them into low bytes and high bytes
    */
    divider = BASE_FREQUENCY / PIT_FREQ;
    //PIT only accepts 8 bit value, we divide them into low bytes and high bytes
    lobyte = divider & 0x00FF; //we mask it wih 00FF to get lower bytes.
    hibyte = divider >> 2*BYTE; //we want to shift it by 8 bit o get high byte
    
    /* 
     * Table for Mode and Command register
     * Bits      Usage
     * 6 and 7   Select channel :
     *           0 0 = Channel 0
     *           0 1 = Channel 1
     *           1 0 = Channel 2
     *           1 1 = Read-back command (8254 only)
     * 4 and 5   Access mode :
     *           0 0 = Latch count value command
     *           0 1 = Access mode: lobyte only
     *           1 0 = Access mode: hibyte only
     *           1 1 = Access mode: lobyte/hibyte
     * 1 to 3    Operating mode :
     *           0 0 0 = Mode 0 (interrupt on terminal count)
     *           0 0 1 = Mode 1 (hardware re-triggerable one-shot)
     *           0 1 0 = Mode 2 (rate generator)
     *           0 1 1 = Mode 3 (square wave generator)
     *           1 0 0 = Mode 4 (software triggered strobe)
     *           1 0 1 = Mode 5 (hardware triggered strobe)
     *           1 1 0 = Mode 2 (rate generator, same as 010b)
     *           1 1 1 = Mode 3 (square wave generator, same as 011b)
     * 0         BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
     */
    outb(0x34, MODE_CMD_REGISTER); //we use channel 0, lobyte/hibyte, and mode 2, that is 0x34
    outb(lobyte, CHANNEL_0);
    outb(hibyte, CHANNEL_0);
    enable_irq(PIT_IRQ);

    return;
}

void pit_handler() {
    scheduler();
}

