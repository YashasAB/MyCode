
/*
 * terminal.c - Implements functions for terminal drivers
 */

#include "lib.h"
#include "terminal.h"
#include "pcb.h"
#include "paging.h"

static void print_char(uint8_t c, terminal_t * term);

/*
 * terminal_init
 * DESCRIPTION: Returns success
 *       INPUT: filename - pointer to filename
 *      RETURN: 0 upon success
 */
void terminal_init() {

  /* Initialize terminal 0 */
  int32_t i;
  for(i = 0; i < BASE_SHELLS; ++i) {
    terminals[i].index = EMPTY_BUF;
    terminals[i].input_flag = WAITING;
    terminals[i].return_flag = WAITING;
    terminals[i].vidmap_flag = NOT_PRESENT;
    terminals[i].term_num = i;
    switch (i) {
      case TERMINAL_0 :
        terminals[i].terminal_video = (uint8_t *) VIDEO_TERM0;
        break;
      case TERMINAL_1 :
        terminals[i].terminal_video = (uint8_t *) VIDEO_TERM1;
        break;
      case TERMINAL_2 :
        terminals[i].terminal_video = (uint8_t *) VIDEO_TERM2;
        break;
    }
    terminals[i].x = terminals[i].y = 0;
  }

  /* set current terminal */
  curr_term = visible_term = &terminals[TERMINAL_0];
}

/*
 * terminal_open
 * DESCRIPTION: Initializes terminal values
 *       INPUT: filename - pointer to filename
 *      RETURN: 0 upon success
 */
int32_t terminal_open(const uint8_t* filename) {
  return SUCCESS;
}

/*
 * terminal_close
 * DESCRIPTION: Does nothing right now
 *       INPUT: fd - file descriptor
 *      RETURN: 0 upon success
 */
int32_t terminal_close(int32_t fd) {
  return FAILURE;
}

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
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {

  uint32_t i;
  int32_t bytes_read = 0;

  if(buf == NULL)
    return 0;

  // enable user buffer input
  curr_term->input_flag = FREE;

  sti();
  while(visible_term->return_flag || curr_term != visible_term);
  cli();
  // disable user buffer input
  visible_term->input_flag = WAITING;

  for(i = 0; i < (nbytes-1) && (i < (BUF_SIZE-1)); ++i) {
    if(visible_term->line_buffer[i] == '\n') {
      break;
    }
    bytes_read++;
    *(((uint8_t *) buf) + sizeof(uint8_t)*i) = visible_term->line_buffer[i];
  }

  // set final char to a line feed character
  bytes_read++;
  *(((uint8_t *) buf) + sizeof(uint8_t)*i) = '\n';

  // prepare for a new read
  visible_term->return_flag = WAITING;
  visible_term->index = EMPTY_BUF;

  return bytes_read;
}

/*
 * terminal_write
 * DESCRIPTION: Writes the data in buf to the terminal
 *       INPUT: fd - file descriptor
 *              buf - buffer that terminal write data from
 *              nbytes - number of bytes to be written
 *      RETURN: number of bytes written upon success
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
  uint32_t i;

  if(buf == NULL)
    return FAILURE;

  if(visible_term == curr_term) {
    for(i = 0; i < nbytes; ++i) {
      putc(*(((uint8_t *) buf) + sizeof(uint8_t)*i));
    }
  }
  else {
    for(i = 0; i < nbytes; ++i) {
      print_char(*(((uint8_t *) buf) + sizeof(uint8_t)*i), curr_term);
    }
  }
  return nbytes;
}

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
int32_t update_terminal_buffer(const int8_t c) {

  if(visible_term->input_flag) {
     return TERMINAL_OFF;
  }

  if(c == '\0') {
    return NULL_CHAR;
  }

  if(c == '\b') {
    if(visible_term->index != EMPTY_BUF) {
      visible_term->line_buffer[visible_term->index] = ' ';
      return visible_term->index--;
    }
    return EMPTY_BUF;
  }

  // leave room for line feed character
  if(visible_term->index == (BUF_SIZE - 1)) {
    if(c == '\n') {
      visible_term->return_flag = FREE;
      visible_term->line_buffer[visible_term->index] = c;
      return visible_term->index;
    }
    return FULL_BUF;
  }
  else {
    visible_term->line_buffer[visible_term->index++] = c;
    if(c == '\n')
      visible_term->return_flag = FREE;
  }
  return visible_term->index;
}

/*
 * clear_terminal
 * DESCRIPTION: Clears the terminal and reprints the current
 *              buffer to the screen
 *       INPUT: NONE
 *      RETURN: NONE
 */
void clear_terminal(void) {
  int32_t i;
  uint8_t shell_prompt[PROMPT_SIZE] = "391OS> ";
  clear();
  if(visible_term->input_flag == FREE) {
    for(i = 0; i < PROMPT_SIZE; ++i) {
      putc(shell_prompt[i]);
    }
  }
  for(i = 0; i < visible_term->index; ++i) {
    putc(visible_term->line_buffer[i]);
  }
}

/* print_char
 * DESCRIPTION: Prints characters to an off-screen terminal
 *       INPUT: c - char
 *              term - terminal to write to
 *      RETURN: NONE
 */
void print_char(uint8_t c, terminal_t * term) {
  if(c == '\0') {
    return;
  }
  else if(c == '\n' || c == '\r') {
    term->y++;
    term->x = 0;
  }
  else if(c=='\t'){
    *(uint8_t *)(term->terminal_video + ((NUM_COLS * term->y + term->x) << 1)) = ' ';
    *(uint8_t *)(term->terminal_video + ((NUM_COLS * term->y + term->x) << 1) + 1) = ATTRIB;
    term->x++;
    term->y = (term->y + term->x / NUM_COLS);
    term->x %= NUM_COLS;
  }
  else {
    *(uint8_t *)(term->terminal_video + ((NUM_COLS * term->y + term->x) << 1)) = c;
    *(uint8_t *)(term->terminal_video + ((NUM_COLS * term->y + term->x) << 1) + 1) = ATTRIB;
    term->x++;
    term->y = (term->y + term->x / NUM_COLS);
    term->x %= NUM_COLS;
  }

  if(term->y == NUM_ROWS) {
    term->y = NUM_ROWS-1;
    uint32_t i;
    memmove(term->terminal_video, (term->terminal_video+(NUM_COLS << 1)), ((NUM_ROWS-1)*NUM_COLS) << 1);
    for (i = (NUM_ROWS-1)*NUM_COLS; i < NUM_ROWS * NUM_COLS; i++) {
      *(uint8_t *)(term->terminal_video + (i << 1)) = ' ';
      *(uint8_t *)(term->terminal_video + (i << 1) + 1) = ATTRIB;
    }
  }
}

/*
 * switch_terminal
 * DESCRIPTION: Switches terminals
 *       INPUT: term_num - number of the terminal to switch to
 *      RETURN: NONE
 */
void switch_terminal(uint8_t term_num) {

  /* copy current screen data into current termial */
  memcpy(visible_term->terminal_video,(uint8_t *)VIDEO,((NUM_ROWS)*NUM_COLS) << 1);
  visible_term->x = get_screen_x();
  visible_term->y = get_screen_y();

  /* copy new terminal memory into video memory */
  memcpy((uint8_t *)VIDEO,terminals[term_num].terminal_video,((NUM_ROWS)*NUM_COLS) << 1);
  set_screen_x(terminals[term_num].x);
  set_screen_y(terminals[term_num].y);
  update_cursor(terminals[term_num].x,terminals[term_num].y);
  
  // remap user video page when we switch terminals. 
  if((visible_term == curr_term) && curr_term->vidmap_flag) {
    terminal_vid((uint32_t)curr_term->terminal_video);
  }

  /* set current terminal to new terminal */
  visible_term = &terminals[term_num];

  if((visible_term == curr_term) && curr_term->vidmap_flag) {
    user_vid_paging();
  }
}
