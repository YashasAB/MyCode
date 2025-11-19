/* rtc.h - Defines functions used in interactions with the RTC
 * (Real Time Clock)
 * vim:ts=4 noexpandtab
 */

#ifndef _RTC_H
#define _RTC_H

#include "types.h"

/* RTC Running Frequency */
#define RTC_FREQ     1024
#define DEFAULT_FREQ 2

/* Default values */
#define WAITING     1
#define INTERRUPT   0
#define INIT_TICKS  0
#define RESET_TICKS INIT_TICKS

/* RTC IRQ number */
#define RTC_IRQ 8

/* RTC ports */
#define RTC_CMD 0x70
#define RTC_REG 0x71

/* RTC commands */
#define SELECT_B_DISABLE_NMI 0x8B
#define SELECT_C_DISABLE_NMI 0x8C

/* RTC bit masks */
#define RTC_INT_ENABLE 0x40

typedef struct rtc {
  uint8_t interrupt;
  uint32_t virtual_freq;
  uint32_t ticks;
} rtc_t;

/*
 * rtc_init
 * DESCRIPTION: Initializes the RTC
 *       INPUT: NONE
 *      RETURN: NONE
 */
void rtc_init(void);

/*
 * rtc_handle
 * DESCRIPTION: Handles interrupts from RTC by updating tick count
 *              and preparing for another interrupt. If the tick count
 *              reach an appropriate level, the interrupt flag will be cleared
 *       INPUT: NONE
 *      RETURN: NONE
 */
void rtc_handle(void);

/*
 * rtc_open
 * DESCRIPTION: Sets virtual frequency to the default frequency and
 *              resets interrupt so that an the interrupt flag will
 *              be cleared 1/virtual_freq seconds after this function
 *              is called
 *       INPUT: filename - pointer to filename
 *      RETURN: 0 upon success
 */
int32_t rtc_open(const uint8_t* filename);

/*
 * rtc_close
 * DESCRIPTION: Does nothing for now
 *       INPUT: fd - file descriptor
 *      RETURN: 0 upon success
 */
int32_t rtc_close(int32_t fd);

/*
 * rtc_read
 * DESCRIPTION: Returns upon receiving an interrupt or if interrupt flag
 *              is already cleared
 *       INPUT: fd - file descriptor
 *              buf - dummy arg here
 *              nbytes - dummy arg here
 *      RETURN: 0 upon success
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);

/*
 * rtc_write
 * DESCRIPTION: Sets virtual frequency of RTC. It must be a power of two,
 *              greater than zero, and less than or equal to 1024
 *       INPUT: fd - file descriptor
 *              buf - pointer to frequency
 *              nbytes - dummy arg here
 *      RETURN: 0 upon succes and -1 upon failure
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);

#endif /* _RTC_H */
