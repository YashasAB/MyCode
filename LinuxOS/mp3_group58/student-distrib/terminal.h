
#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"

#define BUF_SIZE      128
#define WAITING       1
#define FREE          0
#define EMPTY_BUF     0
#define FULL_BUF      0
#define NULL_CHAR     0
#define TERMINAL_OFF  0
#define PROMPT_SIZE   7
#define VIDEO_TERM0   0xB9000
#define VIDEO_TERM1   0xBA000
#define VIDEO_TERM2   0xBB000

/*
 * terminal_init
 * DESCRIPTION: Initializes terminal values
 *       INPUT: filename - pointer to filename
 *      RETURN: 0 upon success
 */
void terminal_init(void);

/*
 * terminal_open
 * DESCRIPTION: Returns SUCESS
 *       INPUT: filename - pointer to filename
 *      RETURN: 0 upon success
 */
int32_t terminal_open(const uint8_t* filename);

/*
 * terminal_close
 * DESCRIPTION: Does nothing right now
 *       INPUT: fd - file descriptor
 *      RETURN: 0 upon success
 */
int32_t terminal_close(int32_t fd);

/*
 * terminal_read
 * DESCRIPTION: Reads the line_buffer into the buf parameter
 *              after a line feed character is pressed by  
 *              the keyboard
 *       INPUT: fd - file descriptor
 *              buf - buffer that terminal will put data into
 *              nbytes - number of bytes to be read
 *      RETURN: number of bytes read
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);

/*
 * terminal_write
 * DESCRIPTION: Writes the data in buf to the terminal
 *       INPUT: fd - file descriptor
 *              buf - buffer that terminal write data from
 *              nbytes - number of bytes to be written
 *      RETURN: 0 upon success
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

/*
 * update_terminal_buffer
 * DESCRIPTION: This function handles the interaction between
 *              the terminal drivers and the keyboard drivers.
 *              Every keypress updates the buffer until it is full,
 *              and the return_flag is cleared when the line feed
 *              character is pressed. Backspace functionality works
 *              as long as there are chars in the buffer
 *       INPUT: c - a character from the keyboard
 *      RETURN: 0 if the buffer is empty or full or the current
 *              index of the buffer otherwise
 */
int32_t update_terminal_buffer(const int8_t c);

/*
 * clear_terminal
 * DESCRIPTION: Clears the terminal and reprints the current
 *              buffer to the screen
 *       INPUT: NONE
 *      RETURN: NONE
 */
void clear_terminal(void);

/*
 * switch_terminal
 * DESCRIPTION: Switches terminals
 *       INPUT: term_num - number of the terminal to switch to
 *      RETURN: NONE
 */
void switch_terminal(uint8_t term_num);

#endif /* _TERMINAL_H */
