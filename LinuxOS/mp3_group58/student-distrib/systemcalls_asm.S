/* filename: systemcalls.S */

#define ASM     1
#include "pcb.h"

.data

  SUCCESS = 0
  FAILURE = -1
  SYS_CALLS = 8

user_ds:
  .long 0x002B

user_esp:
  .long 0x83FFFFC

user_cs:
  .long 0x0023

exec_offset:
  .long 0x08048018

eflags_mask:
  .long 0x00000200

system_call_table:
  .long system_halt, system_execute, system_read, system_write
  .long system_open, system_close, system_getargs, system_vidmap

.text

.globl   scheduler_wrapper
.globl   switch_task
.globl   system_execute
.globl   system_halt
.globl   system_call_wrapper
.align   4

/*
 * scheduler_wrapper
 * DESCRIPTION: Assembly linkage for pit handler/scheduler
 *       INPUT: NONE
 *      RETURN: NONE
 */
scheduler_wrapper:
  pushl  %ebp
  movl   %esp,%ebp
  pushl  %esp
  pushl  %ebp
  call scheduler
switch_task:
  leave
  iret

/*
 * system_execute(const uint8_t * command)
 * DESCRIPTION: Executes a command if it is valid
 *       INPUT: command - executable command
 *      RETURN: Result of executed program
 */
system_execute:
  pushl  %ebp
  movl   %esp,%ebp
  pushl  8(%ebp)
  call   _system_execute
  addl   $4, %esp
  cmpl   $1, %eax
  je     execute_fail_M
  cmpl   $SUCCESS,%eax
  jne    execute_fail

  /* Setup "fake iret" case */
  pushl  user_ds
  pushl  user_esp
  pushfl
  popl   %eax
  orl    eflags_mask, %eax
  pushl  %eax
  pushl  user_cs
  movl   exec_offset,%eax
  pushl  (%eax)
  /**************************/

  iret
execute_fail_M:
  movl $SUCCESS, %eax
  jmp halt
execute_fail:
  movl  $FAILURE,%eax
halt:
  leave
	ret

/*
 * system_halt(uint8_t status)
 * DESCRIPTION: Halts the current program. If the program is
 *              the first shell, the it will restart in the
 *              helper function
 *       INPUT:
 *      RETURN: Reason for the halt
 */
system_halt:
  pushl %ebx
  call _system_halt
  addl $4, %esp
  jmp halt

/*
 * system_call_wrapper()
 * DESCRIPTION: Assembly linkage for system calls
 *       INPUT: EAX - System call number
 *              EBX - System call arg #1
 *              ECX - System call arg #2
 *              EDX - System call arg #3
 *      RETURN: result of system call
 */
system_call_wrapper:
    pushfl
    cmpl  $SYS_CALLS, %eax
    ja    system_call_error
    cmpl  $0, %eax             # zero is outside of system call range
    je    system_call_error
    addl  $-1, %eax            # adjust for 0-indexed jump table
    pushl %edx
    pushl %ecx
    pushl %ebx
    call  *system_call_table(,%eax,4)
    addl  $12,%esp
    jmp   system_call_done
system_call_error:
    movl  $FAILURE,%eax
system_call_done:
    popfl
    iret

/* NEED TO RETURN EAX */
