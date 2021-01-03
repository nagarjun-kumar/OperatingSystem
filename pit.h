#ifndef _PIT_H
#define _PIT_H

#include "scheduler.h"

#define BASE_FREQUENCY 1193182
#define MODE_CMD_REGISTER 0x43
#define CHANNEL_0 0x40
#define PIT_IRQ 0x00
#define PIT_FREQ 100

extern void pit_init(void);
extern void pit_handler(void);

#endif
