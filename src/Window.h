/** Build Windows on Sheets
 * 
 * @author: Leon Wong
 * @build: 201808310443
 * @brief: This file provides functions on window management based on Sheet Management.
 * @usage: This file should be both included and compiled.
 * 
 */ 

#if (!defined(WINDOW_H))
#define WINDOW_H 1

#include "Sheet.h"
#include "VisualFuncs.h"

/**
 * @brief: Make a window with a sheet buffer and other info.
 * @param: (uchar *)buf: Buffer for this window.
 * @param: (int)xsize: X size of the window.
 * @param: (int)ysize: Y size of the window.
 * @param: (char *)title: Title of the window.
 */ 
void make_window8(unsigned char *buf, int xsize, int ysize, char *title);

#endif
