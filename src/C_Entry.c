/**
 * @brief: The Entry of C system files.
 * @author: Leon Wong
 * @build: 201808260241
 * @usage: Be compiled!
 */ 

#include "VisualFuncs.h"
#include "BootInfo.h"
#include "Fonts.h"
#include "asm_funcs.h"
#include "Mouse.h"
#include "GDT.h"
#include "IDT.h"
#include "PIC.h"
#include "include\stdio.h"
#include "include\stdlib.h"

static const char *version = "MewOS 0.0.2.2";

/**
 * ENTRY FUNCTION!
 */ 
void MewOSMain();

/**
 * @brief: Init the OS, for example: initial UI.
 * @param: BootInfo *binfo: The Boot Info got from the asm file.
 * @param: char *mouse: The array to save the mouse picture.
 * @warning: The mouse array should be at lease 256bytes!
 * @seealso: init_mouse_cursor8
 */ 
void do_init(BootInfo *binfo, char *mouse);

void MewOSMain(){
    BootInfo *binfo = (BootInfo *) 0x0ff0;
	//char *mouse = (char *) malloc(256 * sizeof(char));
	char mouse[256];

	do_init(binfo, mouse);

	while (1){
		io_hlt();
	}
}

void do_init(BootInfo *binfo, char *mouse){

	initGDT();
	initIDT();
	initPIC();

    io_sti();

	init_palette();
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
	init_mouse_cursor8(mouse, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, (binfo->scrnx - 16) / 2, // These magic numbers are set to make mouse at the x center
	 (binfo->scrny - 28 - 16) / 2, // These magic numbers are set to make the mouse at the y center (without the bar on the bottom)
	 mouse, 16);

	io_out8(PIC0_IMR, 0xf9);
	io_out8(PIC1_IMR, 0xef);
}
