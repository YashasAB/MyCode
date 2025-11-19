/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = INIT_PIC_MASK; /* IRQs 0-7  */
uint8_t slave_mask = INIT_PIC_MASK;  /* IRQs 8-15 */

/*
 * i8259_init
 * DESCRIPTION: Initialize the PIC
 *       INPUT: NONE
 *      RETURN: NONE
 */
void i8259_init(void) {

	uint32_t flags;
	cli_and_save(flags);
	outb(INIT_PIC_MASK, MASTER_8259_IMR);	/* mask all of 8259A-1 */
	outb(INIT_PIC_MASK, SLAVE_8259_IMR);	/* mask all of 8259A-2 */

	/*
	 * outb_p - this has to work on a wide range of PC hardware.
	 */
	outb_p(ICW1, MASTER_8259_PORT);	/* ICW1: select 8259A-1 init */
	outb_p(ICW2_MASTER , MASTER_8259_IMR);	/* ICW2: 8259A-1 IR0-7 mapped to 0x20-0x27 */
	outb_p(ICW3_MASTER, MASTER_8259_IMR);	/* 8259A-1 (the master) has a slave on IR2 */
	outb_p(ICW4, MASTER_8259_IMR);
	outb_p(ICW1, SLAVE_8259_PORT);	/* ICW1: select 8259A-2 init */
	outb_p(ICW2_SLAVE, SLAVE_8259_IMR);	/* ICW2: 8259A-2 IR0-7 mapped to 0x28-0x2f */
	outb_p(ICW3_SLAVE, SLAVE_8259_IMR);	/* 8259A-2 is a slave on master's IR2 */
	outb_p(ICW4, SLAVE_8259_IMR); /* (slave's support for AEOI in flat mode is to be investigated) */

	outb(master_mask, MASTER_8259_IMR); /* restore master IRQ mask */
	outb(slave_mask, SLAVE_8259_IMR);	  /* restore slave IRQ mask */
	restore_flags(flags);
}

/*
 * enable_irq
 * DESCRIPTION: Enables the IRQ corresponding to irq_num on the PIC
 *       INPUT: irq_num - Number 0-15 corresponding to a PIC IRQ
 *      RETURN: NONE
 */
void enable_irq(uint32_t irq_num) {
	uint32_t flags;

	cli_and_save(flags);
	if (irq_num >= SLAVE_OFFSET) {
		// unmask irq on slave
		slave_mask &= (uint8_t) ~(ENABLE_BIT << (irq_num - SLAVE_OFFSET));
		outb(slave_mask, SLAVE_8259_IMR);
	}
	else {
		//unmask irq on master
		master_mask &= (uint8_t) ~(ENABLE_BIT << irq_num);
		outb(master_mask, MASTER_8259_IMR);
	}
	restore_flags(flags);
}

/*
 * disable_irq
 * DESCRIPTION: Disables IRQ corresponding to irq_num on PIC
 *       INPUT: irq_num - Number 0-15 corresponding to a PIC IRQ
 *      RETURN: NONE
 */
void disable_irq(uint32_t irq_num) {
	uint32_t flags;

	cli_and_save(flags);
	if (irq_num >= SLAVE_OFFSET) {
		// mask irq on slave
		slave_mask |= (uint8_t) (DISABLE_BIT << (irq_num - SLAVE_OFFSET));
		outb(slave_mask, SLAVE_8259_IMR);
	}
	else {
		// mask irq on master
		master_mask |= (uint8_t) (DISABLE_BIT << irq_num);
		outb(master_mask, MASTER_8259_IMR);
	}
	restore_flags(flags);
}

/*
 * send_eoi
 * DESCRIPTION: Sends end-of-interrupt signal to the PIC for a 
 *              corresponding IRQ number
 *       INPUT: irq_num - Number 0-15 corresponding to a PIC IRQ
 *      RETURN: NONE
 */
void send_eoi(uint32_t irq_num) {
	uint32_t flags;

	cli_and_save(flags);
	if (irq_num >= SLAVE_OFFSET) {
		outb(EOI+(irq_num & NUM_OF_PINS),SLAVE_8259_PORT);/* 'Specific EOI' to slave */
		outb(EOI+CASCADE_PIN,MASTER_8259_PORT); /* 'Specific EOI' to master-IRQ2 */
	} 
	else {
		outb(EOI+irq_num,MASTER_8259_PORT);	/* 'Specific EOI to master */
	}
	restore_flags(flags);
}
