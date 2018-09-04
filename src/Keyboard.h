/** Keyboard Management
 * 
 * @author: Leon Wong
 * @build: 201809040928
 * @brief: This file includes functions on keyboard management
 * @usage: This file should be both included and compiled
 * 
 */

#if (!defined(KEYBOARD_H))
#define KEYBOARD_H 1

#include "BootInfo.h"
#include "Fonts.h"
#include "VisualFuncs.h"
#include "asm_funcs.h"
#include "FIFOBuffer.h"
#include "PIC.h"

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
void initKeyboard(FIFO32 *fifo, int data0);

#endif
