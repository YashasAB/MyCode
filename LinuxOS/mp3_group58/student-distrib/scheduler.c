/* scheduler.c - Implements functions used in interactions with the scheduler
 * vim:ts=4 noexpandtab
 */

#include "types.h"
#include "scheduler.h"
#include "pcb.h"
#include "i8259.h"
#include "lib.h"
#include "x86_desc.h"
#include "paging.h"

static uint8_t boot_flag = 0;
extern int32_t system_execute(const uint8_t * command);

/*
 * scheduler_init
 * DESCRIPTION: Initializes the PIT for the scheduler
 *       INPUT: NONE
 *      RETURN: NONE
 */
void scheduler_init(void) {
  enable_irq(PIT_IRQ);
}

/*
 * scheduler
 * DESCRIPTION: Handles interrupts from PIT in order to switch
 *              between tasks running on the three base shells
 *       INPUT: ebp - base pointer from current context
 *              esp - stack pointer from current context
 *      RETURN: NONE
 */
void scheduler(uint32_t ebp, uint32_t esp) {
  uint8_t term_num;

  send_eoi(PIT_IRQ);

  /* Boot cases */
  switch(boot_flag) {

    // first shell
    case TERMINAL_0 :
      boot_flag++;
      curr_term = &terminals[TERMINAL_0];
      system_execute((uint8_t *) "shell");
      break;

    // second shell
    case TERMINAL_1 :
      boot_flag++;

      /* save stack information */
      curr_term->esp = esp;
      curr_term->ebp = ebp;
      curr_term->esp0 = tss.esp0;
      /**************************/

      curr_term = &terminals[TERMINAL_1];
      system_execute((uint8_t *) "shell");
      break;
    
    // third shell
    case TERMINAL_2 :
      boot_flag++;

      /* save stack information */
      curr_term->esp = esp;
      curr_term->ebp = ebp;
      curr_term->esp0 = tss.esp0;
      /**************************/

      curr_term = &terminals[TERMINAL_2];
      system_execute((uint8_t *) "shell");
      break;
  }

  /* Determine next terminal for execution */
  switch(curr_term->term_num) {
    case TERMINAL_0 :
      term_num = TERMINAL_1;
      break;
    case TERMINAL_1 :
      term_num = TERMINAL_2;
      break;
    case TERMINAL_2 :
      term_num = TERMINAL_0;
      break;
  }

  /* save stack information */
  curr_term->esp = esp;
  curr_term->ebp = ebp;
  curr_term->esp0 = tss.esp0;
  /**************************/

  /* Restore previous termina/program information, 
     information, pepare for context switch */
  curr_term = &terminals[term_num];
  pcb = curr_term->curr_pcb;
  tss.esp0 = curr_term->esp0;
  user_paging(curr_term->curr_pcb->pid);
  if ((curr_term == visible_term) && curr_term->vidmap_flag){
    user_vid_paging();
  }
  if ((curr_term != visible_term) && curr_term->vidmap_flag){
    terminal_vid((uint32_t)curr_term->terminal_video);
  }
  /*****************************************/

  asm volatile (
    "movl %0, %%esp;"
    "movl %1, %%ebp;"
    "jmp switch_task"
    :
    : "r" (curr_term->esp), "r" (curr_term->ebp)
    :"esp", "ebp", "cc"
  ); // switch to next task
}
