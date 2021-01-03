/*
 * Wrapper file for System Call
 */
#ifndef _SYSCALL_WRAPPER_H_
#define _SYSCALL_WRAPPER_H_

#include "syscall.h"

#ifndef ASM
    extern void syscall_wrapper();
#endif
#endif
