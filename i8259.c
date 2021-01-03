/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */


#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */
/*
i8259_init
DESCRIPTION: initalizes master and slave pic
INPUTS: none
OUTPUTS: none
RETURN VALUE: none
SIDE EFFECTS: none
*/
/* Initialize the 8259 PIC */
void i8259_init(void) {
    master_mask = 0xFF; 
    slave_mask  = 0xFF;

    outb(master_mask, MASTER_VAL); //mask all interrupts
    outb(slave_mask, SLAVE_VAL); //mask all interrupts

    //initialize master
    outb(ICW1, MASTER_8259_PORT); 
    outb(ICW2_MASTER, MASTER_VAL);
    outb(ICW3_MASTER, MASTER_VAL);
    outb(ICW4, MASTER_VAL);

    //initialize slave
    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_SLAVE, SLAVE_VAL);
    outb(ICW3_SLAVE, SLAVE_VAL);
    outb(ICW4, SLAVE_VAL);
    enable_irq(SLAVE_IRQ);
}
/*
enable_irq
DESCRIPTION: enable (unmask) the specified IRQ
Inputs: irq_num - number of irq to be enabled
Outputs: none
Return Value: none
SIDE EFFECTS: none
*/
void enable_irq(uint32_t irq_num) {
    uint16_t loc;
    if (irq_num < IRQ_VAL){ //number of irq in one pic
        loc = MASTER_VAL; //irq on master pic
        master_mask &= ~(1 << irq_num); //update bitmask
        outb(master_mask, loc); 

    } else {
        loc = SLAVE_VAL; //irq on slave pic
        irq_num -= IRQ_VAL; //makr irq_num to be in the range of 0-8
        slave_mask &= ~(1 << irq_num); //update bitmask
        outb(slave_mask, loc);
    }
}
/*
disable_irq
DESCRIPTION:  Disable (mask) the specified IRQ
INPUTS: irq_num - number of irq that has to be enabled
OUTPUTS: none
RETURN VALUES: none
SIDE EFFECTS: none
*/
void disable_irq(uint32_t irq_num) {
    uint16_t loc;
    if (irq_num < IRQ_VAL){ //check if slave or master
        loc = MASTER_VAL; 
        master_mask |= 1 <<  irq_num; //update bitmask
        outb(master_mask, loc);
    } else {
        loc = SLAVE_VAL; //slave pic
        irq_num  -= IRQ_VAL; //make irq_num in range of 0-8
        slave_mask |= 1 << irq_num; //update bitmask
        outb(slave_mask, loc); 
    }
}
/*
send_eoi
DESCRIPTION: Send end-of-interrupt signal for the specified IRQ
INPUTS: irq_num - number of irq to be enabled
OUTPUTS: none
RETURN VALUES: none
SIDE EFFECTS: none
*/
void send_eoi(uint32_t irq_num) {
    if (irq_num >= IRQ_VAL){//check if irq is on slave pic
        outb(EOI | (irq_num - IRQ_VAL), SLAVE_8259_PORT);
        outb(EOI | SLAVE_IRQ, MASTER_8259_PORT);
    } else {
        outb(EOI | irq_num, MASTER_8259_PORT); //irq on masterpic
    }
}
