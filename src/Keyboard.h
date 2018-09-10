/** Keyboard Management
 * 
 * @author: Leon Wong
 * @build: 201809110020 FINAL
 * @brief: This file includes functions on keyboard management
 * @usage: This file should be both included and compiled
 * 
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H 1

#include "BootInfo.h"
#include "Fonts.h"
#include "VisualFuncs.h"
#include "asm_funcs.h"
#include "FIFOBuffer.h"
#include "PIC.h"

extern int keydata0;

/// Port of keyboard.
const static int PORT_KEYBOARD = 0x0060;

/// Port to get the status of keyboard
const static int PORT_KEYBOARD_STAT = 0x0064;

/// Port to send keyboard cmds
const static int PORT_KEYBOARD_CMD = 0x0064;

/// Keyboard will return this if not ready
const static int KEYBOARD_STAT_NOTREADY = 0x02;

/// Define keyboard cmd write mode
const static int KEYBOARD_CMD_WRITE_MODE = 0x60;

/// Keyboard mode with mouse control
const static int KEYBOARD_MODE_KBC = 0x47;

/// Keyboard LED control
const static int KEYBOARD_CMD_LED = 0xed;

/// Indices of function keys
const static int KEY_TAB = 0x0f;
const static int KEY_LEFT_SHIFT = 0x2a;
const static int KEY_RIGHT_SHIFT = 0x36;
const static int KEY_LEFT_SHIFT_RELEASED = 0xaa;
const static int KEY_RIGHT_SHIFT_RELEASED = 0xb6;
const static int KEY_CAPSLOCK = 0x3a;
const static int KEY_NUMLOCK = 0x45;
const static int KEY_SCROLLLOCK = 0x46;
const static int KEY_SEND_SUCCESS = 0xfa;
const static int KEY_SEND_FAIL = 0xfe;
const static int KEY_ENTER = 0x1c;
const static int KEY_BACKSPACE = 0x0e;
const static int KEY_F2 = 0x3c;
const static int KEY_F11 = 0x57;

/**
 * @brief: Handle interrupts from keyboard.
 * @param: int *eap: ESP Register
 * @seealso: inthandler21
 */ 
void keyboardInterruptHandler(int *esp);

/**
 * @brief: Init the keyboard buffer with an array of uchars.
 * @param: (int) *buf: The array of the buffer.
 * @param: int size: Size of the buffer.
 */
// __DEPRECATED
void initKeyboardBuffer(int *buf, int size);

/**
 * @brief: Call this function will pause the current process and wait until keyboard is ready.
 */ 
void waitKeyboardReady();

/**
 * @brief: Init keyboard control with mouse.
 * @param: (FIFO32 *)fifo: The FIFO buffer for keyboard.
 * @param: (int)data0: The constant that the IRQ data from keyboard will add.
 */ 
void initKeyboard(struct FIFOBuffer32 *fifo, int data0);

/// Keyboard table
const static char keyboardTable[0x80] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 0, 
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0, '\\', 
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0, 
    // Fn keys starts here. (10 keys)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Numpad
    0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0x5c, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x5c, 0, 0
};

const static char keyboardTableCapitalized[0x80] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', 0, 
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 
    // Fn keys starts here. (10 keys)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Numpad
    0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0x5c, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x5c, 0, 0
};

#endif
