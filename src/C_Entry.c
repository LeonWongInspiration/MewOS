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
#include "Sheet.h"

#include "include\stdio.h"

static const char *version = "MewOS 0.1.0.0";

extern FIFO8 keyboardBuffer;
extern FIFO8 mouseBuffer;

/**
 * ENTRY FUNCTION!
 */ 
void MewOSMain();

void MewOSMain(){
	BootInfo *binfo = (BootInfo *) ADR_BOOTINFO; // Get boot info first.
	char s[40]; // A tmp string array to sprintf strings.
	char keyboardBuf[32]; // Buffer for keyboard.
	char mouseBuf[128]; // Buffer for mouse.
	int mx, my; // Mouse init position.
	int i;
	unsigned int totalMemory; // Total memory capacity.
	MOUSE_DECODER mdec;
	MEMORY_FREE_TABLE *memman = (MEMORY_FREE_TABLE *) MEMORY_MANAGER_ADDR;
	SHEET_MANAGER *shtctl;
	SHEET *sheetBackground;
	SHEET *sheetMouse;
	unsigned char *bufferBackground;
	unsigned char bufferMouse[256];

	initGDT();
	initIDT();
	initPIC();

	io_sti();
	initKeyboardBuffer(keyboardBuf, 32);
	initMouseBuffer(mouseBuf, 128);
	io_out8(PIC0_IMR, 0xf9); // Enable PIC1
	io_out8(PIC1_IMR, 0xef);

	initKeyboard();
	enableMouse(&mdec);

	totalMemory = memtest(0x00400000, 0xbfffffff);
	memoryFreeTableInit(memman);
	freeMemoryWithAddrAndSize(memman, 0x00001000, 0x0009e000);
	freeMemoryWithAddrAndSize(memman, 0x00400000, totalMemory - 0x00400000);

	init_palette();
	shtctl = initSheetManager(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	sheetBackground = allocASheetForWindow(shtctl);
	sheetMouse = allocASheetForWindow(shtctl);
	bufferBackground = (unsigned char*) allocMemoryForSize_Page(memman, binfo->scrnx * binfo->scrny);
	setSheetBuffer(sheetBackground, bufferBackground, binfo->scrnx, binfo->scrny, -1);
	setSheetBuffer(sheetMouse, bufferMouse, 16, 16, 99); // 16 is the width and height of mouse, 99 means mouse is very high.
	init_screen(bufferBackground, binfo->scrnx, binfo->scrny);
	init_mouse_cursor8(bufferMouse, 99); // Now the bg color for mouse is not very important.
	sheetMove(shtctl, sheetBackground, 0, 0);
	mx = (binfo->scrnx - 16) / 2; // Put the mouse at the center of the screen,
	my = (binfo->scrny - 28 - 16) / 2; // and without the bar at the bottom.
	sheetMove(shtctl, sheetMouse, mx, my);
	setSheetHeight(shtctl, sheetBackground, 0);
	setSheetHeight(shtctl, sheetMouse, 1);
	sprintf(s, "(%3d, %3d)", mx, my);
	putfonts8_asc(bufferBackground, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
	sprintf(s, "Total Memory %dMB, Free : %dKB",
			totalMemory / (1024 * 1024), getAvailableMemorySpace(memman) / 1024);
	putfonts8_asc(bufferBackground, binfo->scrnx, 0, 32, COL8_FFFFFF, s);
	sheetRefresh(shtctl, sheetBackground, 0, 0, binfo->scrnx, 48);
	
	//putfonts8_asc(bufferBackground, binfo->scrnx, 0, 128, COL8_FFFFFF, "Init finished.");
	//sheetRefresh(shtctl, sheetBackground, 0, 0, binfo->scrnx, 48);

	while (1){
		io_cli();
		if ((fifo8_status(&keyboardBuffer) | fifo8_status(&mouseBuffer)) == 0){
			// No IRQs from keyboard and mouse.
			//putfonts8_asc(bufferBackground, binfo->scrnx, 0, 128, COL8_FFFFFF, "NO IRQs found.");
			//sheetRefresh(shtctl, sheetBackground, 0, 0, binfo->scrnx, 48);
			io_stihlt();
		}
		else {
			unsigned char c;
			if (fifo8_status(&keyboardBuffer) != 0) {
				if (fifo8_get(&keyboardBuffer, &c) == 0){
					io_sti();
					sprintf(s, "%02X", c);
					boxfill8(bufferBackground, binfo->scrnx, COL8_008484,  0, 16, 15, 31);
					putfonts8_asc(bufferBackground, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
					sheetRefresh(shtctl, sheetBackground, 0, 16, 16, 32);
				}
				else {
					io_sti();
				}
			}
			else if (fifo8_status(&mouseBuffer) != 0) {
				if (fifo8_get(&mouseBuffer, &c) == 0) {
					io_sti();
					if (mouseDecode(&mdec, c) != 0) {
						sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
						if ((mdec.btn & 0x01) != 0) {
							s[1] = 'L';
						}
						if ((mdec.btn & 0x02) != 0) {
							s[3] = 'R';
						}
						if ((mdec.btn & 0x04) != 0) {
							s[2] = 'C';
						}
						boxfill8(bufferBackground, binfo->scrnx, COL8_008484, 32, 16, 32 + 15 * 8 - 1, 31);
						putfonts8_asc(bufferBackground, binfo->scrnx, 32, 16, COL8_FFFFFF, s);
						sheetRefresh(shtctl, sheetBackground, 32, 16, 32 + 15 * 8, 32);
						mx += mdec.x;
						my += mdec.y;
						if (mx < 0) {
							mx = 0;
						}
						if (my < 0) {
							my = 0;
						}
						// Allow mouse to be hid when reaching right/bottom
						if (mx > binfo->scrnx - 1) {
							mx = binfo->scrnx - 1;
						}
						if (my > binfo->scrny - 1) {
							my = binfo->scrny - 1;
						}
						sprintf(s, "(%3d, %3d)", mx, my);
						boxfill8(bufferBackground, binfo->scrnx, COL8_008484, 0, 0, 79, 15);
						putfonts8_asc(bufferBackground, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
						sheetRefresh(shtctl, sheetBackground, 0, 0, 80, 16);
						sheetMove(shtctl, sheetMouse, mx, my);
					}
				}
				else {
					io_sti();
				}
			}
		}
	}
}
