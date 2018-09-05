/**
 * @brief: The Entry of C system files.
 * @author: Leon Wong
 * @build: 201808040956
 * @usage: Be compiled!
 */ 

#define __STOP while(1){io_hlt();}

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
#include "Window.h"
#include "Timer.h"
#include "Widgets.h"

#include "include\stdio.h"

static const char *version = "MewOS 0.2.1.0";

/**
 * ENTRY FUNCTION!
 */ 
void MewOSMain();

void MewOSMain(){

	BootInfo *binfo = (BootInfo *) ADR_BOOTINFO;
	FIFO32 fifo; // Unified FIFO buffer for IRQs.
	char s[40]; // For sprintf.
	int fifobuf[128];
	TIMER *timer, *timer2, *timer3;
	int mx, my; // Position for mouse.
	int i;
	int cursorX, cursorColor; // Position and color for cursor.
	// int count = 0;
	unsigned int totalMemory;
	MOUSE_DECODER mdec;
	MEMORY_FREE_TABLE *memman = (MEMORY_FREE_TABLE *) MEMORY_MANAGER_ADDR;
	SHEET_MANAGER *sheetManager;
	SHEET *sheetBackground;
	SHEET *sheetMouse;
	SHEET *sheetWindow;
	unsigned char *bufferBackground;
	unsigned char bufferMouse[256];
	unsigned char *bufferWindow;

	// ------ Do system init ------ //
	initGDT();
	initIDT();
	initPIC();
	io_sti(); // Now GDT IDT has been inited, we can allow interrupts.
	fifo32_init(&fifo, 128, fifobuf);
	initPIT();
	initKeyboard(&fifo, 256);
	enableMouse(&fifo, &mdec, 512);

	// ------ Do IRQ allow ------ //
	io_out8(PIC0_IMR, 0xf8); // Enable interrupts from devices (0xf8)
	io_out8(PIC1_IMR, 0xef);

	// ------ Do timers init ------ //
	timer = allocTimer();
	initTimer(timer, &fifo, 10);
	timerSetTimeOut(timer, 1000); // Timeout: 10s

	timer2 = allocTimer();
	initTimer(timer2, &fifo, 3);
	timerSetTimeOut(timer2, 300); // Timeout: 3s

	timer3 = allocTimer();
	initTimer(timer2, &fifo, 1);
	timerSetTimeOut(timer3, 50);

	// ------ Do memory init ------ //
	totalMemory = memtest(0x00400000, 0xbfffffff);
	memoryFreeTableInit(memman);
	freeMemoryWithAddrAndSize(memman, 0x00001000, 0x0009e000); // Assign memory free to the table
	freeMemoryWithAddrAndSize(memman, 0x00400000, totalMemory - 0x00400000); // What between the two parts are our system!

	// ------ Do sheets init ------ //
	init_palette();
	sheetManager = initSheetManager(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	sheetBackground = allocASheetForWindow(sheetManager);
	sheetMouse = allocASheetForWindow(sheetManager);
	sheetWindow = allocASheetForWindow(sheetManager);
	bufferBackground = (unsigned char*) allocMemoryForSize_Page(memman, binfo->scrnx * binfo->scrny);
	bufferWindow = (unsigned char*) allocMemoryForSize_Page(memman, 160 * 52);
	setSheetBuffer(sheetBackground, bufferBackground, binfo->scrnx, binfo->scrny, -1);
	setSheetBuffer(sheetMouse, bufferMouse, 16, 16, 99);
	setSheetBuffer(sheetWindow, bufferWindow, 160, 52, -1);

	init_screen(bufferBackground, binfo->scrnx, binfo->scrny);
	init_mouse_cursor8(bufferMouse, 99);
	make_window8(bufferWindow, 160, 52, "TextArea");
	make_textbox8(sheetWindow, 8, 28, 144, 16, COL8_FFFFFF);
	cursorX = 8;
	cursorColor = COL8_FFFFFF;

	sheetMove(sheetBackground, 0, 0);
	mx = (binfo->scrnx - 16) / 2;
	my = (binfo->scrny - 28 - 16) / 2;
	sheetMove(sheetMouse, mx, my);
	sheetMove(sheetWindow, 80, 72);
	setSheetHeight(sheetBackground, 0);
	setSheetHeight(sheetWindow, 1);
	setSheetHeight(sheetMouse, 2);

	sprintf(s, "(%3d, %3d)", mx, my);
	putStringOnSheet(sheetBackground, 0, 0, COL8_FFFFFF, COL8_008484, s, 10);
	sprintf(s, "memory %dMB   free : %dKB",
			totalMemory / (1024 * 1024), getAvailableMemorySpace(memman) / 1024);
	putStringOnSheet(sheetBackground, 0, 32, COL8_FFFFFF, COL8_008484, s, 40);

	// ------ Timer debug ------ //
	if (timer == NULL) { putStringOnSheet(sheetBackground, 0, 128, COL8_FFFF00, COL8_000000, "Timer is NULL", 13); }
	if (timer2 == NULL) { putStringOnSheet(sheetBackground, 0, 128, COL8_FFFF00, COL8_000000, "Timer2 is NULL", 14); }
	if (timer3 == NULL) { putStringOnSheet(sheetBackground, 0, 128, COL8_FFFF00, COL8_000000, "Timer3 is NULL", 14); }

	// ------ Interrupt handling ------ //
	while (1){
		io_cli();
		if (fifo32_status(&fifo) == 0) {
			io_stihlt();
		}
		else {
			fifo32_get(&fifo, &i);
			io_sti();
			if (i >= keydata0 && i < mousedata0){
				// ------ Show key info on background ------ //
				sprintf(s, "%02X", i - keydata0);
				putStringOnSheet(sheetBackground, 0, 16, COL8_FFFFFF, COL8_008484, s, 2);
				
				// ------ Show key data on window ------ //
				if (i < 0x54 + keydata0) {
					if (keyboardTable[i - keydata0] != 0 && cursorX < 144 && keyboardTable[i - keydata0] != '\b') { // We get a character and have enough space to show it.
						s[0] = keyboardTable[i - keydata0];
						s[1] = '\0';
						putStringOnSheet(sheetWindow, cursorX, 28, COL8_000000, COL8_FFFFFF, s, 1);
						cursorX += 8;
					}
					if (i == keydata0 + 0x0e && cursorX > 8){ // If key i is backspace, and there is something to delete.
						putStringOnSheet(sheetWindow, cursorX, 28, COL8_000000, COL8_FFFFFF, " ", 1);
						cursorX -= 8;
					}
					boxfill8(sheetWindow->buf, cursorX, cursorColor, cursorX, 28, cursorX + 7, 43);
					sheetRefresh(sheetWindow, cursorX, 28, cursorX + 8, 44);
				}
			}
			else if (i >= mousedata0 && i <= 767) {
				if (mouseDecode(&mdec, i - mousedata0) != 0) {
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
					putStringOnSheet(sheetBackground, 32, 16, COL8_FFFFFF, COL8_008484, s, 15);
					mx += mdec.x;
					my += mdec.y;
					if (mx < 0) {
						mx = 0;
					}
					if (my < 0) {
						my = 0;
					}
					if (mx > binfo->scrnx - 1) {
						mx = binfo->scrnx - 1;
					}
					if (my > binfo->scrny - 1) {
						my = binfo->scrny - 1;
					}
					sprintf(s, "(%3d, %3d)", mx, my);
					putStringOnSheet(sheetBackground, 0, 0, COL8_FFFFFF, COL8_008484, s, 10);
					sheetMove(sheetMouse, mx, my);

					if ((mdec.btn & 0x01) != 0){ // If mouse left key is pressed down.
						sheetMove(sheetWindow, mx - 80, my - 8);
					}
				}
			}
			else if (i == 10){
				putStringOnSheet(sheetBackground, 0, 64, COL8_FFFFFF, COL8_008484, "10 sec", 6);
			}
			else if (i == 3){
				putStringOnSheet(sheetBackground, 0, 64, COL8_FFFFFF, COL8_008484, "3 sec", 5);
			}
			else if (i <= 1){ // Change the color of the cursor.
				if (i != 0) {
					initTimer(timer3, &fifo, 0);
					cursorColor = COL8_000000;
				}
				else {
					initTimer(timer3, &fifo, 1);
					cursorColor = COL8_FFFFFF;
				}
				timerSetTimeOut(timer3, 50); // Cursor will change its color every 500 ms.
				boxfill8(sheetWindow->buf, sheetWindow->bxsize, cursorColor, cursorX, 28, cursorX + 7, 43);
				sheetRefresh(sheetWindow, cursorX, 28, cursorX + 8, 44);
			}
		}
	}
}
