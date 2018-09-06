/** Functions To Show Texts
 * 
 * @author: Leon Wong
 * @build: 201808250330
 * @brief: This file includes an array of fonts and functions to show texts on the screen.
 * @usage: This file should be both included and compiled.
 * 
 */

#ifndef FONTS_H
#define FONTS_H 1

/// Font array, this array is linked from MewFont.obj file, in which ascii chars are defined in 8*16 matrices.
extern char MewFont[4096];

/**
 * @brief: Put a character on the screen.
 * @param: char *vram: The address of the VRAM.
 * @param: int xsize: The width of a char.
 * @param: int x: The x value where the char starts.
 * @param: int y: The y value where the char starts.
 * @param: char c: Color.
 * @param: char *font: The character's font.
 * @seealso: putfonts8_asc
 */ 
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);

/**
 * @brief: Put a string on the screen.
 * @param: char *vram: The address of the VRAM.
 * @param: int xsize: The width of a char.
 * @param: int x: The x value where the string starts.
 * @param: int y: The y value where the string starts.
 * @param: char c: Color.
 * @param: uchar* s: The C-Style string
 * @warning: This function puts a C-Style string, and recognizes the end of the string by \0!
 * @seealso: putfont8
 */
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);

#endif
