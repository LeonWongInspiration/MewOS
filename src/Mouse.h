/** Functions All About Mouse
 * 
 * @author: Leon Wong
 * @build: 201808300143
 * @brief: This file includes all functions about mouse.
 * @usage: This file should be both included and compiled.
 * 
 */

#if (!defined(MOUSE_H))
#define MOUSE_H 1

#include "VisualFuncs.h"
#include "Keyboard.h"
#include "asm_funcs.h"
#include "BootInfo.h"
#include "PIC.h"
#include "FIFOBuffer.h"

/// Keyboard cmd send to mouse.
const static int KEYBOARD_CMD_SENDTO_MOUSE = 0xd4;

/// CMD to enable mouse.
const static int MOUSECMD_ENABLE = 0xf4;

typedef struct MOUSE_DECODER{
    unsigned char decodeBuffer[3]; // Because mouse data are in 3 bytes, we need a 3 byte array to contain them.
    unsigned char phase; // Goes as 0, 1, 2, 3, 1, 2, 3, 1, 2, 3 ... to indicate the stat of mouse data.
                        // Note that when phase == 0, we are waiting for mouse to get ready (0xfa).
    int x, y; // Position.
    int btn; // Stat of the buttons.
} MOUSE_DECODER;

/**
 * @brief: Init a mouse cursor UI
 * @param: char *mouse: The address to save the picture for mouse.
 * @param: char bc: Background color.
 * @warning: char *mouse should be a char array with at least 256 bytes!
 */ 
void init_mouse_cursor8(char *mouse, char bc);

/**
 * @brief: Init keyboard control to get mouse ready.
 * @param: (MOUSE_DECODER *)mouseDecoder: The decoder strauct of the mouse.
 * @param: (int)data0: The constant that the data from mouse will add.
 */ 
void enableMouse(MOUSE_DECODER *mouseDecoder, int data0);

/**
 * @brief: Handle interrupts from Mouse.
 * @param: int *eap: ESP Register
 * @seealso: inthandler2c
 */ 
void mouseInterruptHandler(int *esp);

/**
 * @brief: Init the mouse buffer with an array of uchars.
 * @param: (int *)buf: The array of the buffer.
 * @param: int size: Size of the buffer.
 */ 
void initMouseBuffer(int *buf, int size);

/**
 * @brief: Decode a series of mouse data.
 * @param: (MOUSE_DECODER *)mouseDecoder: The MOUSE_DECODER struct.
 * @param: (uchar) dat: Data to be saved in the struct. 
 * @return: (int): If succeed in reading a data, you'll get 0, if succeed in reading a 3-byte array, you'll get 1. -1 if failed.
 */ 
int mouseDecode(MOUSE_DECODER *mouseDecoder, unsigned char dat);

#endif
