#include "idt.h"
#include "types.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "rtc.h"
#include "syscall.h"
#include "keyboard.h"
#include "syscall_wrapper.h"

#define EXCEPTION(name,msg)	\
void name() {				\
	printf("%s\n",#msg);	\
	halt_flag = 1;			\
	halt(0);				\
}

/* Exceptions sent to the exception creator macro. */
EXCEPTION(exception1,"Divide Error!");
EXCEPTION(exception2,"Debug Exception!");
EXCEPTION(exception3,"Non Maskable Interrupt Exception!");
EXCEPTION(exception4,"Breakpoint Exception!");
EXCEPTION(exception5,"Overflow Exception!");
EXCEPTION(exception6,"BOUND Range Exceeded Exception!");
EXCEPTION(exception7,"Invalid Opcode Exception!");
EXCEPTION(exception8,"Device Not Available Exception!");
EXCEPTION(exception9,"Double Fault Exception!");
EXCEPTION(exception10,"Coprocessor Segment Exception!");
EXCEPTION(exception11,"Invalid TSS Exception!");
EXCEPTION(exception12,"Segment Not Present!");
EXCEPTION(exception13,"Stack Fault Exception!");
EXCEPTION(exception14,"General Protection Exception!");
EXCEPTION(exception15,"Page Fault Exception!");
EXCEPTION(exception16,"Floating Point Exception");
EXCEPTION(exception17,"Alignment Check Exception!");
EXCEPTION(exception18,"Machine Check Exception!");
EXCEPTION(exception19,"SIMD Floating-Point Exception!");

/*general interruption
*description: a general one that not defined in table
* input: none
* output: none
* return value: none
*/
void general_interruption(){
	cli();
	printf("Undefined interruption!");
}

/* init_idt
* description: initialize the interrupts
* input: none
* output: unsuccessful: 1, successful: 0
*/
void init_idt(){
	int idx = 0;
	lidt(idt_desc_ptr);
	for(idx = 0; idx < NUM_VEC; idx++){
		idt[idx].present = 0x01;
		idt[idx].dpl = 0x00;
		idt[idx].size = 0x01;
		idt[idx].reserved0 = 0x00;
		idt[idx].reserved1 = 0x01;
		idt[idx].reserved2 = 0x01;
		idt[idx].reserved3 = 0x00;
		idt[idx].reserved4 = 0x00;
		idt[idx].seg_selector = KERNEL_CS;
		if(idx < 32){
			idt[idx].reserved3 = 0x00;
		}
		if(SYSCALL_VECTOR == idx){
			idt[idx].dpl = 0x03;
		}
	}
	SET_IDT_ENTRY(idt[0], exception1);
	SET_IDT_ENTRY(idt[1], exception2);
	SET_IDT_ENTRY(idt[2], exception3);
	SET_IDT_ENTRY(idt[3], exception4);
	SET_IDT_ENTRY(idt[4], exception5);
	SET_IDT_ENTRY(idt[5], exception6);
	SET_IDT_ENTRY(idt[6], exception7);
	SET_IDT_ENTRY(idt[7], exception8);
	SET_IDT_ENTRY(idt[8], exception9);
	SET_IDT_ENTRY(idt[9], exception10);
	SET_IDT_ENTRY(idt[10], exception11);
	SET_IDT_ENTRY(idt[11], exception12);
	SET_IDT_ENTRY(idt[12], exception13);
	SET_IDT_ENTRY(idt[13], exception14);
	SET_IDT_ENTRY(idt[14], exception15);	
	SET_IDT_ENTRY(idt[16], exception16);
	SET_IDT_ENTRY(idt[17], exception17);
	SET_IDT_ENTRY(idt[18], exception18);
	SET_IDT_ENTRY(idt[19], exception19);
	SET_IDT_ENTRY(idt[KEYBOARD_INDEX], KEYBOARD_INTERRUPT);
	SET_IDT_ENTRY(idt[RTC_INDEX], RTC_INTERRUPT);
	SET_IDT_ENTRY(idt[SYSCALL_INDEX], syscall_wrapper);
	SET_IDT_ENTRY(idt[PIT_INDEX], PIT_INTERRUPT);
}

