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
#include "Keyboard.h"
#include "FIFOBuffer.h"
#include "include\stdio.h"
#include "include\stdlib.h"

static const char *version = "MewOS 0.0.2.2";

extern FIFO8 keyboardBuffer;

/**
 * ENTRY FUNCTION!
 */ 
void MewOSMain();

/**
 * Reset some buffers, keyboard, etc.
 */ 
void resetBuffers();

/**
 * @brief: Init the OS, for example: initial UI.
 * @param: BootInfo *binfo: The Boot Info got from the asm file.
 * @param: char *mouse: The array to save the mouse picture.
 * @param: uchar *keyboardBuffer: The buffer allocated for keyboard FIFO8 Buffer.
 * @warning: The mouse array should be at lease 256bytes!
 * @seealso: init_mouse_cursor8
 */ 
void do_init(BootInfo *binfo, char *mouse, unsigned char *keyboardBuffer);

void MewOSMain(){
    BootInfo *binfo = (BootInfo *) 0x0ff0;
	//char *mouse = (char *) malloc(256 * sizeof(char));
	char mouse[256];
	char keyboardBuffer[32];

	do_init(binfo, mouse, keyboardBuffer);

	//putfonts8_asc(binfo->vram, binfo->scrnx, 0, 80, COL8_840084, "Prepare to call resetBuffers");

	while (1){
		resetBuffers(binfo);
	}
}

void do_init(BootInfo *binfo, char *mouse, unsigned char *keyboardBuffer){

	initGDT();
	initIDT();
	initPIC();

    io_sti();

	// Init keyboard buffer.
	initKeyboardBuffer(keyboardBuffer, 32);

	io_out8(PIC0_IMR, 0xf9);
	io_out8(PIC1_IMR, 0xef);

	init_palette();
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
	init_mouse_cursor8(mouse, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, (binfo->scrnx - 16) / 2, // These magic numbers are set to make mouse at the x center
	 (binfo->scrny - 28 - 16) / 2, // These magic numbers are set to make the mouse at the y center (without the bar on the bottom)
	 mouse, 16);

	//putfonts8_asc(binfo->vram, binfo->scrnx, 0, 80, COL8_840084, "Init finished");
}

void resetBuffers(BootInfo *binfo){
	io_cli(); // Temporaly disallow interrupts.
	if (fifo8_status(&keyboardBuffer) == 0) {
		io_stihlt(); // Cannot be replace with io_sti();io_cli();
					 // Or, interrupts coming between the two instructions will be ignored.
		//putfonts8_asc(binfo->vram, binfo->scrnx, 0, 60, COL8_840084, "Keyboard Not Available");
	} else {
		//putfonts8_asc(binfo->vram, binfo->scrnx, 0, 60, COL8_840084, "Reset Buffers Called");
		unsigned char c;
		io_sti();
		char s[40];
		fifo8_get(&keyboardBuffer, &c);
		sprintf(s, "%02X", (int)c);
		boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
		putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
	}
}
