/** Keyboard Management
 * 
 * @author: Leon Wong
 * @build: 201808270410
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

/**
 * @brief: Handle interrupts from keyboard.
 * @param: int *eap: ESP Register
 * @seealso: inthandler21
 */ 
void keyboardInterruptHandler(int *esp);

/**
 * Init the keyboard buffer with an array of uchars.
 * @param: uchar *buf: The array of the buffer.
 * @param: int size: Size of the buffer.
 */ 
void initKeyboardBuffer(unsigned char *buf, int size);

#endif
