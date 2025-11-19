/* rtc.c - Implements functions used in interactions with the RTC
 * (Real Time Clock)
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "i8259.h"
#include "lib.h"
#include "pcb.h"

static rtc_t rtc[BASE_SHELLS];

/*
 * rtc_init
 * DESCRIPTION: Initializes the RTC
 *       INPUT: NONE
 *      RETURN: NONE
 */
void rtc_init(void) {
  uint32_t i;
  outb(SELECT_B_DISABLE_NMI,RTC_CMD);     // select register B, and disable NMI
  uint8_t prev = inb(RTC_REG);	          // read the current value of register B
  outb(SELECT_B_DISABLE_NMI, RTC_CMD);		// set the index again (a read will reset the index to register D)
  outb((prev | RTC_INT_ENABLE), RTC_REG);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
  
  /* Initialize starting values */
  for(i = 0; i < BASE_SHELLS; ++i) {
    rtc[i].interrupt = WAITING;
    rtc[i].virtual_freq = DEFAULT_FREQ;
    rtc[i].ticks = INIT_TICKS;
  }

  enable_irq(CASCADE_PIN);
  enable_irq(RTC_IRQ);
}

/*
 * rtc_handle
 * DESCRIPTION: Handles interrupts from RTC by updating tick count
 *              and preparing for another interrupt. If the tick count
 *              reach an appropriate level, the interrupt flag will be cleared
 *       INPUT: NONE
 *      RETURN: NONE
 */
void rtc_handle(void) {
  uint8_t term_num;
  for(term_num = 0; term_num <BASE_SHELLS; term_num++) {
    rtc[term_num].ticks++;
  }
  term_num = curr_term->term_num;
  if(rtc[term_num].ticks >= (RTC_FREQ/rtc[term_num].virtual_freq)) {
    rtc[term_num].interrupt = INTERRUPT;
    rtc[term_num].ticks = RESET_TICKS;
  }
  outb(SELECT_C_DISABLE_NMI, RTC_CMD);
  inb(RTC_REG);
  send_eoi(RTC_IRQ);
}

/*
 * rtc_open
 * DESCRIPTION: Sets virtual frequency to the default frequency and
 *              resets interrupt so that an the interrupt flag will
 *              be cleared 1/virtual_freq seconds after this function
 *              is called. Returns an unused file descriptor.
 *       INPUT: filename - pointer to filename
 *      RETURN: file descriptor upon success, -1 upon failure
 */
int32_t rtc_open(const uint8_t* filename) {

  // set up file_desc_t for rtc
  int32_t fd;
  uint8_t term_num = curr_term->term_num;
  for(fd = FILE_ARRAY_START; fd < FILE_ARRAY_LENGTH; ++fd) {
    if(pcb->file_desc_table[fd].flags == UNUSED) {
      break;
    }
  } 
  if(fd == FILE_ARRAY_LENGTH) {
    return FAILURE;
  }
  rtc[term_num].virtual_freq = DEFAULT_FREQ;
  rtc[term_num].interrupt = WAITING;
  rtc[term_num].ticks = INIT_TICKS;
  return fd;
}

/*
 * rtc_close
 * DESCRIPTION: Does nothing for now
 *       INPUT: fd - file descriptor
 *      RETURN: 0 upon success
 */
int32_t rtc_close(int32_t fd) {
  return SUCCESS;
}

/*
 * rtc_read
 * DESCRIPTION: Returns upon receiving an interrupt or if interrupt flag
 *              is already cleared
 *       INPUT: fd - file descriptor
 *              buf - dummy arg here
 *              nbytes - dummy arg here
 *      RETURN: 0 upon success
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) {

  // wait for RTC interrupt
  sti();
  while(rtc[curr_term->term_num].interrupt);
  cli();

  // Prepare for next interrupt
  rtc[curr_term->term_num].interrupt = WAITING;
  rtc[curr_term->term_num].ticks = RESET_TICKS;
  return SUCCESS;
}

/*
 * rtc_write
 * DESCRIPTION: Sets virtual frequency of RTC. It must be a power of two,
 *              greater than zero, and less than or equal to 1024
 *       INPUT: fd - file descriptor
 *              buf - pointer to frequency
 *              nbytes - dummy arg here
 *      RETURN: 0 upon succes and -1 upon failure
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes) {

  uint32_t freq = *((uint32_t *) buf);

  // check if the frequency is not zero, a power of two, and not greater
  // than the maximum RTC_FREQ
  if((freq != 0) && ((freq & (freq - 1)) == 0) && (freq <= RTC_FREQ)) {
    rtc[curr_term->term_num].virtual_freq = freq;
    return SUCCESS;
  }
  return FAILURE;
}
