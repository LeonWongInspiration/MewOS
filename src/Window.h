/** Build Windows on Sheets
 * 
 * @author: Leon Wong
 * @build: 201809102322 FINAL
 * @brief: This file provides functions on window management based on Sheet Management.
 * @usage: This file should be both included and compiled.
 * 
 */ 

#ifndef WINDOW_H
#define WINDOW_H 1

#include "Sheet.h"
#include "VisualFuncs.h"
#include "Fonts.h"
#include "FIFOBuffer.h"

/**
 * @brief: Make a window with a sheet buffer and other info.
 * @param: (uchar *)buf: Buffer for this window.
 * @param: (int)xsize: X size of the window.
 * @param: (int)ysize: Y size of the window.
 * @param: (char *)title: Title of the window.
 * @param: (char)act: Indicator of whether this window is now the front one (thus its title's color will be darker).
 */ 
void make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act);

/**
 * @brief: Draw the title of a window separately from the body of the window.
 * @param: (uchar *)buf: The buffer of the window.
 * @param: (int)xsize: X size of the window.
 * @param: (int)ysize: Y size of the window.
 * @param: (char *)title: The title of the window.
 * @param: (char)act: Whether this window is in the front. (i.e. currently running)
 */ 
void make_wtitle8(unsigned char *buf, int xsize, char *title, char act);

/**
 * @brief: Manage the window's title color according to mouse.
 * @param: (SHEET *)key_win: The window now receiving keys.
 */ 
//void keywin_off(struct SHEET *key_win, struct SHEET *sht_win, int cur_c, int cur_x);
void keywin_off(struct SHEET *key_win);

/**
 * @brief: Set the window's color when clicked.
 * @param: (SHEET *)key_win: The window now receiving keys.
 */
//void keywin_on(struct SHEET *key_win, struct SHEET *sht_win, int cur_c);
void keywin_on(struct SHEET *key_win);

/**
 * @brief: Change the color of the window's title without considering its string on title.
 * @param: (SHEET *)sht: The sheet to change color.
 * @param: (char)act: Whether the window is active.
 */
void change_wtitle8(struct SHEET *sht, char act); 

#endif
