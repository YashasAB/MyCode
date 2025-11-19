/* i8259.h - Defines used in interactions with the 8259 interrupt
 * controller
 * vim:ts=4 noexpandtab
 */

#ifndef _I8259_H
#define _I8259_H

#include "types.h"

/* Ports that each PIC sits on */
#define MASTER_8259_PORT    0x20
#define SLAVE_8259_PORT     0xA0

/* IMR port for each PIC */
#define MASTER_8259_IMR     0x21
#define SLAVE_8259_IMR      0xA1

/* Initialization control words to init each PIC.
 * See the Intel manuals for details on the meaning
 * of each word */
#define ICW1                0x11
#define ICW2_MASTER         0x20
#define ICW2_SLAVE          0x28
#define ICW3_MASTER         0x04
#define ICW3_SLAVE          0x02
#define ICW4                0x01

/* Bit-wise constants */
#define INIT_PIC_MASK       0xFF
#define SLAVE_OFFSET        0x08
#define ENABLE_BIT          0x01
#define DISABLE_BIT         ENABLE_BIT
#define NUM_OF_PINS         7

/* Potentially spurious IRQs */
#define SPURIOUS_IRQ1       7
#define SPURIOUS_IRQ2       15

/* Slave cascade pin on master PIC */
#define CASCADE_PIN         0x02

/* End-of-interrupt byte.  This gets OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished */
#define EOI                 0x60

/* Externally-visible functions */

/*
 * i8259_init
 * DESCRIPTION: Initialize the PIC
 *       INPUT: NONE
 *      RETURN: NONE
 */
void i8259_init(void);

/*
 * enable_irq
 * DESCRIPTION: Enables the IRQ corresponding to irq_num on the PIC
 *       INPUT: irq_num - Number 0-15 corresponding to a PIC IRQ
 *      RETURN: NONE
 */
void enable_irq(uint32_t irq_num);

/*
 * disable_irq
 * DESCRIPTION: Disables IRQ corresponding to irq_num on PIC
 *       INPUT: irq_num - Number 0-15 corresponding to a PIC IRQ
 *      RETURN: NONE
 */
void disable_irq(uint32_t irq_num);

/*
 * send_eoi
 * DESCRIPTION: Sends end-of-interrupt signal to the PIC for a 
 *              corresponding IRQ number
 *       INPUT: irq_num - Number 0-15 corresponding to a PIC IRQ
 *      RETURN: NONE
 */
void send_eoi(uint32_t irq_num);

#endif /* _I8259_H */
