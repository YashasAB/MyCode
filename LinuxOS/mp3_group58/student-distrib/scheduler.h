/* scheduler.h - Defines functions used in interactions with the scheduler
 * vim:ts=4 noexpandtab
 */

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#define PIT_IRQ   0

/*
 * pit_init
 * DESCRIPTION: Initializes the PIT for the scheduler
 *       INPUT: NONE
 *      RETURN: NONE
 */
void scheduler_init(void);

/*
 * scheduler
 * DESCRIPTION: Handles interrupts from PIT in order to switch
 *              between tasks running on the three base shells
 *       INPUT: ebp - base pointer from current context
 *              esp - stack pointer from current context
 *      RETURN: NONE
 */
void scheduler(uint32_t ebp, uint32_t esp);

#endif /* _SCHEDULER_H */
