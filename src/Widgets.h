/** Make Widgets For Windows
 * 
 * @author: Leon Wong
 * @build: 201809042250
 * @brief: This file contains functions making widgets for windows such as buttons, textboxes.
 * @usage: This file should be both included and compiled!
 * 
 */

#ifndef WIDGETS_H
#define WIDGETS_H 1

#include "Sheet.h"
#include "VisualFuncs.h"

/**
 * @brief: Draw a textbox on a window sheet buffer.
 * @param: (SHEET *)sht: The sheet to draw textbox on.
 * @param: (int)x0: The x position of the box (left top).
 * @param: (int)y0: The y position of the box (left top).
 * @param: (int)sx: The width of the box.
 * @param: (int)sy: The height of the box.
 * @param: (int)c: The background color of the window.
 */ 
void make_textbox8(SHEET *sht, int x0, int y0, int sx, int sy, int c);

#endif
