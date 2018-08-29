/**
 * @brief: The Entry of C system files.
 * @author: Leon Wong
 * @build: 201808300145
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
#include "Memorie.h"

#include "include\stdio.h"
#include "include\stdlib.h"

static const char *version = "MewOS 0.0.3.1";

extern FIFO8 keyboardBuffer;
extern FIFO8 mouseBuffer;
extern MOUSE_DECODER mouseDecoder;

/**
 * ENTRY FUNCTION!
 */ 
void MewOSMain();

/**
 * Reset some buffers, keyboard, etc.
 */ 
void resetBuffers(BootInfo *binfo);

/**
 * @brief: Init the OS, for example: initial UI.
 * @param: BootInfo *binfo: The Boot Info got from the asm file.
 * @param: char *mouse: The array to save the mouse picture.
 * @param: uchar *keyboardBuffer: The buffer allocated for keyboard FIFO8 Buffer.
 * @warning: The mouse array should be at lease 256bytes!
 * @seealso: init_mouse_cursor8
 */ 
void do_init(BootInfo *binfo, char *mouse, unsigned char *keyboardBuffer, unsigned char *mouseBuffer);

void MewOSMain(){
    BootInfo *binfo = (BootInfo *) 0x0ff0;
	//char *mouse = (char *) malloc(256 * sizeof(char));
	char mouse[256];
	char keyboardBufferArray[32];
	char mouseBufferArray[128];

	do_init(binfo, mouse, keyboardBufferArray, mouseBufferArray);

	int i = memtest(0x00400000, 0xbfffffff) / (1024 * 1024);
	char tmp[40];
	sprintf(tmp, "Available memory: %dMB", i);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, tmp);

	//putfonts8_asc(binfo->vram, binfo->scrnx, 0, 80, COL8_840084, "Prepare to call resetBuffers");

	int theSecretOfTheUniverse = 42;

	while (theSecretOfTheUniverse == 42){
		resetBuffers(binfo);
	}
}

void do_init(BootInfo *binfo, char *mouse, unsigned char *keyboardBufferArray, unsigned char *mouseBufferArray){

	initGDT();
	initIDT();
	initPIC();

    io_sti();

	// Init keyboard buffer.
	initKeyboardBuffer(keyboardBufferArray, 32);

	// Init mouse buffer.
	initMouseBuffer(mouseBufferArray, 128);
	// Init MouseDecoder
	mouseDecoder.decodeBuffer[0] = 0;
	mouseDecoder.decodeBuffer[1] = 0;
	mouseDecoder.decodeBuffer[2] = 0;
	mouseDecoder.btn = 0;
	mouseDecoder.phase = 0;
	mouseDecoder.x = (binfo->scrnx - 16) / 2;
	mouseDecoder.y = (binfo->scrny - 28 - 16) / 2;

	io_out8(PIC0_IMR, 0xf9);
	io_out8(PIC1_IMR, 0xef);

	initKeyboard();

	init_palette();
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
	init_mouse_cursor8(mouse, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, (binfo->scrnx - 16) / 2, // These magic numbers are set to make mouse at the x center
	 (binfo->scrny - 28 - 16) / 2, // These magic numbers are set to make the mouse at the y center (without the bar on the bottom)
	 mouse, 16);

	enableMouse(&mouseDecoder);

	// putfonts8_asc(binfo->vram, binfo->scrnx, 0, 80, COL8_840084, "Init finished");
}

void resetBuffers(BootInfo *binfo){
	io_cli(); // Temporaly disallow interrupts.

	char s[40]; // A buffer to show strings.

	// Deal with keyboard & mouse buffer.
	if (fifo8_status(&keyboardBuffer) + fifo8_status(&mouseBuffer) == 0) {
		// putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, "No KB&Mouse IRQ found");
		io_stihlt();
	} else {
		if (fifo8_status(&keyboardBuffer) != 0) {
			unsigned char i; 
			fifo8_get(&keyboardBuffer, &i);
			io_sti();
			sprintf(s, "%02X", i);
			boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
			putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
		}
		else if (fifo8_status(&mouseBuffer) != 0) {
			unsigned char i; 
			fifo8_get(&mouseBuffer, &i);
			io_sti();
			if (mouseDecode(&mouseDecoder, i) != 0) {
				sprintf(s, "[lcr %4d %4d]", mouseDecoder.x, mouseDecoder.y);
				if ((mouseDecoder.btn & 0x01) != 0) {
					s[1] = 'L';
				}
				if ((mouseDecoder.btn & 0x02) != 0) {
					s[3] = 'R';
				}
				if ((mouseDecoder.btn & 0x04) != 0) {
					s[2] = 'C';
				}
				boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 32, 16, 32 + 15 * 8 - 1, 31);
				putfonts8_asc(binfo->vram, binfo->scrnx, 32, 16, COL8_FFFFFF, s);
			}
		}
	}
}
