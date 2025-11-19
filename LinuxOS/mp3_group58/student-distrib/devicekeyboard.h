#include "lib.h"
#include "i8259.h"
#include "types.h"
#include "terminal.h"

/* Keyboard device information */
#define NUM_KEYS         128
#define KEYBOARD_IRQ     1
#define KEYBOARD_PORT    0x60

/* Bit-wise operational values */
#define REVEAL_FBYTE     0xFF

/* Character values */
#define RELEASE_KEY      0x80
#define L_SHIFT          0x2A
#define R_SHIFT          0x36
#define CAPS             0x3A
#define L_SHIFT_RELEASE  (L_SHIFT + RELEASE_KEY)
#define R_SHIFT_RELEASE  (R_SHIFT + RELEASE_KEY)
#define CAPS_RELEASE     (CAPS + RELEASE_KEY)
#define TAB_RELEASE      (0x0F + RELEASE_KEY)
#define CONTROL          0x1D
#define CONTROL_RELEASE  (CONTROL + RELEASE_KEY)
#define ALT              0x38
#define ALT_RELEASE      (ALT + RELEASE_KEY)
#define LETTER_L         0x26
#define LETTER_C         46
#define F1               0x3B
#define F2               0x3C
#define F3               0x3D

/* Flag constants */
#define KEY_ON           1
#define KEY_OFF          0

/* Constants to verify caps and shift flags */
#define STATUS_OFF       0     
#define STATUS_SHIFT     1
#define STATUS_CAPS      2
#define STATUS_BOTH      3
#define CAPS_BIT_SHIFT   1

/*
 * keyboard_init
 * DESCRIPTION: Initializes the keyboard
 *       INPUT: NONE
 *      RETURN: NONE
 */
void keyboard_init(void);

/*
 * handle_keyboard
 * DESCRIPTION: Handles keyboard inputs. Inputs are printed to a line buffer
 *              and printed to the screen
 *       INPUT: NONE
 *      RETURN: NONE
 */
void handle_keyboard(void);
