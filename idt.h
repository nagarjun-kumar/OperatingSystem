#ifndef IDT_H
#define IDT_H

#include "types.h"
#define RTC_INDEX 0x28
#define KEYBOARD_INDEX 0x21
#define SYSCALL_INDEX 0x80
#define PIT_INDEX 0x20

#define SYSCALL_VECTOR		0x80

void init_idt();
void RTC_INTERRUPT();
void KEYBOARD_INTERRUPT();
void PIT_INTERRUPT();
#endif
