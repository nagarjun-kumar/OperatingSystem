/*
 * Header File for Real-Time Clock Support
 */

#include "i8259.h"
#include "fs_abstraction.h" /* Supports file sys abstraction */

#define RTC_PORT 0x70
#define CMOS_PORT 0x71
#define REGISTER_A 0x8A
#define REGISTER_B 0x8B
#define REGISTER_C 0x8C
#define BIT_SIX 0x40
#define BYTE 4
#define MAX_FREQ 1024
#define MIN_FREQ 2
#define REGISTER_C_NMI 0x0C
#define RTC_IRQ 8
#define MAX_RATE 15
#define LOW_BYTE 0x0F
#define HIGH_BYTE 0xF0

volatile int rtc_interrupt_occured;

/* Initializes the RTC for usage. */
extern void rtc_init(void);
/* RTC Interrupt handler */
extern void rtc_handler(void);
/* RTC read funtion */
extern int32_t rtc_read(int32_t* fd, uint32_t* offset,  char* buf, uint32_t nbytes);
/* RTC write function */
extern int32_t rtc_write(int32_t* fd, uint32_t* offset, const char* buf, uint32_t nbytes);
/* open the RTC */
extern int32_t rtc_open(int32_t* fd, char* filename);
/* close the RTC */
extern int32_t rtc_close(int32_t* fd);


/* Jump table for file sys abstraction */
extern fs_jump_table_t rtc_jmptable;
