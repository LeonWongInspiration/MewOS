/** Functions to Modify UI
 * 
 * @author: Leon Wong
 * @build: 201808250330
 * @brief: This file includes some functions which can change the data in the VRAM, thus change what shows on the screen.
 * @usage: This file should be both included and compiled.
 * 
 */ 

#if (!defined(VISUAL_FUNCS_H))
#define VISUAL_FUNCS_H 1

/**
 * @brief: Init the palette which includes 16 colors we use.
 * @seealso: set_palette
 */
void init_palette();

/**
 * @brief: Set the palette in the VBIOS
 * @param: int start: The start index of the palette
 * @param: int end: The end index of the palette
 * @param: uchar *rgb: The palette, which should be set in the form of init_palette func.
 * @warning: Both start and end index will be set into palette!
 * @seealso: init_palette
 */ 
void set_palette(int start, int end, unsigned char *rgb);

/**
 * @brief: Show a square onto the screen.
 * @param: uchar *vram: The address of the VRAM
 * @param: int xsize: X size of the screen which helps to move the cursor.
 * @param: uchar c: Color.
 * @param: int x0: x0 position of the square.
 * @param: int y0: y0 position of the square.
 * @param: int x1: x1 position of the square.
 * @param: int y1: y1 position of the square.
 * @warning: The two pairs of the points should be the left top and right bottom of the square, but don't require to get x0 < x1!
 */ 
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);

/**
 * @brief: Init the screen with default UI.
 * @param: char *vram: The address of VRAM.
 * @param: int x: x resolution.
 * @param: int y: y resolution.
 * @warning: The resolution should be positive!
 */ 
void init_screen(char *vram, int x, int y);

// Color RGB consts
const static int COL8_000000 = 0;
const static int COL8_FF0000 = 1;
const static int COL8_00FF00 = 2;
const static int COL8_FFFF00 = 3;
const static int COL8_0000FF = 4;
const static int COL8_FF00FF = 5;
const static int COL8_00FFFF = 6;
const static int COL8_FFFFFF = 7;
const static int COL8_C6C6C6 = 8;
const static int COL8_840000 = 9;
const static int COL8_008400 = 10;
const static int COL8_848400 = 11;
const static int COL8_000084 = 12;
const static int COL8_840084 = 13;
const static int COL8_008484 = 14;
const static int COL8_848484 = 15;

#endif