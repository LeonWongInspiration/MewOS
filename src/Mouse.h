/** Functions All About Mouse
 * 
 * @author: Leon Wong
 * @build: 201808260145
 * @brief: This file includes all functions about mouse.
 * @usage: This file should be both included and compiled.
 * 
 */

#if (!defined(MOUSE_H))
#define MOUSE_H 1

#include "VisualFuncs.h"

/**
 * @brief: Init a mouse cursor UI
 * @param: char *mouse: The address to save the picture for mouse.
 * @param: char bc: Background color.
 * @warning: char *mouse should be a char array with at least 256 bytes!
 */ 
void init_mouse_cursor8(char *mouse, char bc);

#endif
