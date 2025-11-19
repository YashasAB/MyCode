
#include "devicekeyboard.h"
#include "pcb.h"

static uint32_t keycode;
static uint8_t shift_status;
static int8_t caps_status;
static uint8_t control_status;
static uint8_t alt_status;

extern int32_t system_halt(uint8_t status);

//This is an array where each index corresponds to a keycode pressed.
//The char in each index of this array is the string char that corresponds to the respective keycode

static uint8_t kcode2normal[NUM_KEYS] = {
  //Row 1 of keyboard
  0,  '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
  '\b', '\t',
   'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
   '[', ']',
   '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
   ';', '\'', '`',   0,
 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
 ',', '.', '/', 0,	'*', 0, ' ',
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,'-', 0,	0, 0,	'+',
  0,0,0,0,0,0,0,0, 0, 0, 0,
};

static uint8_t kcode2caps[NUM_KEYS] = {
  //Row 1 of keyboard
  0,  '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
  '[', ']', '\n',
  0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
  ';', '\'', '`',   0,
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
  ',', '.', '/', 0,	'*', 0, ' ',
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,'-', 0,	0, 0,	'+',
  0,0,0,0,0,0,0,0, 0, 0, 0,
};

static uint8_t kcode2shift[NUM_KEYS] = {
  0,  '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
  '{', '}', '\n',
  0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
  ':', '\"', '~',   0,
 '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
 '<', '>', '?', 0,	'*', 0, ' ',
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,'-', 0,	0, 0,	'+',
  0,0,0,0,0,0,0,0, 0, 0, 0,
};

static uint8_t kcode2shiftcaps[NUM_KEYS] = {
  0,  '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
  '{', '}', '\n',
  0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
  ':', '\"', '~',   0,
 '|', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
 '<', '>', '?', 0,	'*', 0, ' ',
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,'-', 0,	0, 0,	'+',
  0,0,0,0,0,0,0,0, 0, 0, 0,
};



/*
 * keyboard_init
 * DESCRIPTION: Initializes the keyboard
 *       INPUT: NONE
 *      RETURN: NONE
 */
void keyboard_init(void) {
  //enable keyboard irq to start receiving keyboard interrupt
  shift_status   = KEY_OFF;
  control_status = KEY_OFF;
  caps_status    = KEY_OFF;
  alt_status     = KEY_OFF;
  enable_irq(KEYBOARD_IRQ);
}

/*
 * handle_keyboard
 * DESCRIPTION: Handles keyboard inputs. Inputs are printed to a line buffer
 *              and printed to the screen
 *       INPUT: NONE
 *      RETURN: NONE
 */
void handle_keyboard(void){

  // variable that identifies if caps lock is on, shift is on,
  // or both are on
  uint32_t status;

  // Tells which set of keycodes to use depending on
  // caps lock and shift
  uint8_t * keycodes;

  //get input from the keyboard port
  keycode = inb(KEYBOARD_PORT);

  //extract the first byte
  keycode = keycode & REVEAL_FBYTE;

  //we want to echo char to screen
  //update term buf will return index if valid thing can be printed there or
  if(keycode > 0 && keycode < RELEASE_KEY) {

    // first check if shift or control or caps have been pressedto set the modes of the keyboard
    switch (keycode) {
      case L_SHIFT : 
        shift_status = KEY_ON;
        break;
      case R_SHIFT : 
        shift_status = KEY_ON;
        break;
      case CONTROL :
        control_status = KEY_ON;
        break;
      case ALT :
        alt_status = KEY_ON;
        break;
    }

    // if control was not released yet and l was pressed, clear the screen for any other thing do nothing
    if(control_status || alt_status) {
      if(keycode == LETTER_L && control_status != 0) {
        clear_terminal();
      }
      if(keycode == F1 && alt_status != 0) {
        switch_terminal(TERMINAL_0);
      }
      if(keycode == F2 && alt_status != 0) {
        switch_terminal(TERMINAL_1);
      }
      if(keycode == F3 && alt_status != 0) {
        switch_terminal(TERMINAL_2);
      }
    }

    //if control isnt pressed any more proceed to print char from the respective buffer
    //choose the buffer based on the status
    else if((keycode != L_SHIFT) && (keycode != R_SHIFT) && (keycode != CAPS)) {

      status = shift_status + (caps_status << CAPS_BIT_SHIFT);

      switch(status) {
        case STATUS_BOTH :
          keycodes = kcode2shiftcaps;
          break;
        case STATUS_CAPS :
          keycodes = kcode2caps;
          break;
        case STATUS_SHIFT :
          keycodes = kcode2shift;
          break;
        case STATUS_OFF :
          keycodes = kcode2normal;
          break;
      }
      if(update_terminal_buffer(keycodes[keycode])) 
        putc(keycodes[keycode]);
    }
  }
  else {
    switch (keycode) {
      case CAPS_RELEASE : 
        caps_status = !caps_status;
        break;
      case L_SHIFT_RELEASE :
        shift_status = KEY_OFF;
        break;
      case R_SHIFT_RELEASE :
        control_status = KEY_OFF;
        break;
      case CONTROL_RELEASE :
        control_status = KEY_OFF;
        break;
      case ALT_RELEASE :
        alt_status = KEY_OFF;
        break;
    }
  }
  send_eoi(KEYBOARD_IRQ);
}
