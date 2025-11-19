/* filename: interrupt.S */

.data

.text

.globl scheduler_wrapper
.globl rtc_wrapper
.globl keyboard_wrapper
.align 4

/*
 * rtc_wrapper
 * DESCRIPTION: Assembly linkage for rtc_handle function
 *              in rtc.c 
 *       INPUT: NONE
 *      RETURN: NONE
 */
rtc_wrapper:
    pushal
    call rtc_handle
    popal
    iret

/*
 * keyboard_wrapper
 * DESCRIPTION: Assembly linkage for hand_keyboard function
 *              in devicekeyboard.c 
 *       INPUT: NONE
 *      RETURN: NONE
 */
keyboard_wrapper:
    pushal
    call handle_keyboard
    popal
    iret
