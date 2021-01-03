#define ASM 1

.globl syscall_wrapper

syscall_wrapper:
    PUSHFL
    pushl %ebp
    pushl %esi
    pushl %edi     # Save callee-saved registers
    pushl %edx     # Last Argument
    pushl %ecx     # Second Argument
    pushl %ebx     # First Argumemt

    cmpl $0, %eax   # Number has to be in range 1 - 11
    jle invalid_syscall
    cmpl $11, %eax
    jge invalid_syscall
    movl syscall_jmptable(, %eax, 4), %eax
    call *%eax
    jmp end_syscall

invalid_syscall:
    movl $-1, %eax  # eax stores -1

end_syscall:
    popl %ebx
    popl %ecx
    popl %edx      # Pop arguments

    popl %edi
    popl %esi
    popl %ebp      #Restore callee-saved registers
    POPFL

    iret

syscall_jmptable:
    .long 0x0
    .long halt
    .long execute
    .long read
    .long write
    .long open
    .long close
    .long getargs
    .long vidmap
    #.long set_handler
    #.long sigreturn
