/**
 * @brief: The Entry of C system files.
 * @author: Leon Wong
 * @build: 201808040956
 * @usage: Be compiled!
 */ 

#define __STOP while(1){io_hlt();}

#include "VisualFuncs.h"
#include "Structs.h"
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
#include "Console.h"

#include "include\stdio.h"

static const char *version = "MewOS 0.2.1.0";

/**
 * ENTRY FUNCTION!
 */ 
void MewOSMain();

void MewOSMain(){

	BootInfo *binfo = (BootInfo *) ADR_BOOTINFO;
	FIFO32 fifo; // Unified FIFO buffer for IRQs.
	FIFO32 keycmd; // Keyboard commands.
	char s[40]; // For sprintf.
	int fifobuf[128];
	int keycmdbuf[32];
	TIMER *timer;
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
	SHEET *sheetConsole;
	unsigned char *bufferBackground;
	unsigned char bufferMouse[256];
	unsigned char *bufferWindow;
	unsigned char *bufferConsole;
	TASK *taskA, *taskConsole;
	int keyTo = 0;
	int keyShift = 0;
	int keyLed = (binfo->leds >> 4) & 7;
	int keycmdWait = -1;

	// ------ Do system init ------ //
	initGDT();
	initIDT();
	initPIC();
	io_sti(); // Now GDT IDT has been inited, we can allow interrupts.
	fifo32_init(&fifo, 128, fifobuf, NULL);
	initPIT();
	initKeyboard(&fifo, 256);
	enableMouse(&fifo, &mdec, 512);

	// ------ Do IRQ allow ------ //
	io_out8(PIC0_IMR, 0xf8); // Enable interrupts from devices (0xf8)
	io_out8(PIC1_IMR, 0xef);
	fifo32_init(&keycmd, 32, keycmdbuf, NULL);

	// ------ Do memory init ------ //
	totalMemory = memtest(0x00400000, 0xbfffffff);
	memoryFreeTableInit(memman);
	freeMemoryWithAddrAndSize(memman, 0x00001000, 0x0009e000); // Assign memory free to the table
	freeMemoryWithAddrAndSize(memman, 0x00400000, totalMemory - 0x00400000); // What between the two parts are our system!

	// ------ Do sheets and tasks init ------ //
	init_palette();
	sheetManager = initSheetManager(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	taskA = initTask(memman);
	fifo.task = taskA;
	runTask(taskA, 1, 2);

	// ------ Background sheet init ------ //
	sheetBackground = allocASheetForWindow(sheetManager);
	bufferBackground = (unsigned char*) allocMemoryForSize_Page(memman, binfo->scrnx * binfo->scrny);
	setSheetBuffer(sheetBackground, bufferBackground, binfo->scrnx, binfo->scrny, -1);
	init_screen(bufferBackground, binfo->scrnx, binfo->scrny);

	// ------ Console sheet init ------ //
	sheetConsole = allocASheetForWindow(sheetManager);
	bufferConsole = (unsigned char *)allocMemoryForSize_Page(memman, 256 * 165);
	setSheetBuffer(sheetConsole, bufferConsole, 256, 165, -1);
	make_window8(bufferConsole, 256, 165, "Console", 0);
	make_textbox8(sheetConsole, 8, 28, 240, 128, COL8_000000);
	taskConsole = allocTask();
	taskConsole->tss.esp = allocMemoryForSize_Page(memman, 64 * 1024) + 64 * 1024 - 8;
	taskConsole->tss.eip = (int) &consoleTask;
	taskConsole->tss.es = 1 * 8;
	taskConsole->tss.cs = 2 * 8;
	taskConsole->tss.ss = 1 * 8;
	taskConsole->tss.ds = 1 * 8;
	taskConsole->tss.fs = 1 * 8;
	taskConsole->tss.gs = 1 * 8;
	*((int *) (taskConsole->tss.esp + 4)) = (int) sheetConsole; // "Variable passing"
	runTask(taskConsole, 2, 2);

	// ------ Window sheet init ------ //
	sheetWindow = allocASheetForWindow(sheetManager);
	bufferWindow = (unsigned char*) allocMemoryForSize_Page(memman, 160 * 52);
	setSheetBuffer(sheetWindow, bufferWindow, 144, 52, -1);
	make_window8(bufferWindow, 144, 52, "some task", 1);
	make_textbox8(sheetWindow, 8, 28, 128, 16, COL8_FFFFFF);
	cursorX = 8;
	cursorColor = COL8_FFFFFF;
	timer = allocTimer();
	initTimer(timer, &fifo, 1);
	timerSetTimeOut(timer, 50);

	// ------ Mouse sheet init ------ //
	sheetMouse = allocASheetForWindow(sheetManager);
	setSheetBuffer(sheetMouse, bufferMouse, 16, 16, 99);
	init_mouse_cursor8(bufferMouse, 99);
	mx = (binfo->scrnx - 16) / 2;
	my = (binfo->scrny - 28 - 16) / 2;
	
	// ------ Arrange initial windows ------ //
	sheetMove(sheetBackground, 0, 0);
	sheetMove(sheetConsole, 32, 4);
	sheetMove(sheetWindow, 64, 56);
	sheetMove(sheetMouse, mx, my);

	setSheetHeight(sheetBackground, 0);
	setSheetHeight(sheetConsole, 1);
	setSheetHeight(sheetWindow, 2);
	setSheetHeight(sheetMouse, 3);

	sprintf(s, "(%3d, %3d)", mx, my);
	putStringOnSheet(sheetBackground, 0, 0, COL8_FFFFFF, COL8_008484, s, 10);
	sprintf(s, "memory %dMB   free : %dKB",
			totalMemory / (1024 * 1024), getAvailableMemorySpace(memman) / 1024);
	putStringOnSheet(sheetBackground, 0, 32, COL8_FFFFFF, COL8_008484, s, 40);

	// ------ Timer debug ------ //
	//if (timer == NULL) { putStringOnSheet(sheetBackground, 0, 128, COL8_FFFF00, COL8_000000, "Timer is NULL", 13); }

	// ------ Init keyboard cmd management ------ //
	fifo32_put(&keycmd, KEYBOARD_CMD_LED);
	fifo32_put(&keycmd, keyLed);

	// ------ Interrupt handling ------ //
	while (1){
		if (fifo32_status(&keycmd) > 0 && keycmdWait < 0) {
			// Now we have data to send to keyboard controller.
			fifo32_get(&keycmd, &keycmdWait);
			waitKeyboardReady();
			io_out8(PORT_KEYBOARD, keycmdWait);
		}

		// ------ Start universal fifo buffer interrupts handling ------ //
		io_cli();
		if (fifo32_status(&fifo) == 0) {
			// No interrupts now, just sleep.
			setTaskSleep(taskA);
			io_sti();
		}
		else {
			fifo32_get(&fifo, &i);
			io_sti();
			if (i >= keydata0 && i < mousedata0) {
				sprintf(s, "%02X", i - keydata0);
				putStringOnSheet(sheetBackground, 0, 16, COL8_FFFFFF, COL8_008484, s, 2);
				if (i < 0x80 + keydata0) {
					// First deal with shift key.
					if (keyShift == 0) {
						s[0] = keyboardTable[i - keydata0];
					}
					else {
						s[0] = keyboardTableCapitalized[i - keydata0];
					}
				}
				else {
					s[0] = '\0';
				}
				if (s[0] >= 'A' && s[0] <= 'Z') {
					// If the key is a letter, decide whether it is capitalized.
					if ( ((keyLed & 4) == 0 && keyShift == 0) // Neither CapsLock nor Shift pressed.
					||
					((keyLed & 4) != 0 && keyShift != 0) // Both CapsLock and Shift pressed.
					) {
						s[0] += 'a' - 'A'; // Change to lowercase.
					}
				}
				if (s[0] != 0) {
					// If the key is characters or symbols.
					if (keyTo == 0) {
						// Send the key to taskA.
						if (cursorX < 128) {
							// Show a character and move the cursor.
							s[1] = '\0';
							putStringOnSheet(sheetWindow, cursorX, 28, COL8_000000, COL8_FFFFFF, s, 1);
							cursorX += 8;
						}
					}
					else {
						fifo32_put(&(taskConsole->fifo), s[0] + keydata0);
					}
				}
				// ------ Now deal with function keys ------ //
				if (i == keydata0 + KEY_TAB) {
					if (keyTo == 0) {
						keyTo = 1; // Change front window
						make_wtitle8(bufferWindow, sheetWindow->bxsize, "some task", 0);
						make_wtitle8(bufferConsole, sheetConsole->bxsize, "Console", 1);
					}
					else {
						keyTo = 0;
						make_wtitle8(bufferWindow, sheetWindow->bxsize, "some task", 1);
						make_wtitle8(bufferConsole, sheetConsole->bxsize, "Console", 0);
					}
					sheetRefresh(sheetWindow, 0, 0, sheetWindow->bxsize, 21);
					sheetRefresh(sheetConsole, 0, 0, sheetConsole->bxsize, 21);
				}
				if (i == keydata0 + KEY_LEFT_SHIFT) {
					keyShift |= 1;
				}
				if (i == keydata0 + KEY_RIGHT_SHIFT) {
					keyShift |= 2;
				}
				if (i == keydata0 + KEY_LEFT_SHIFT_RELEASED) {
					keyShift &= ~1;
				}
				if (i == keydata0 + KEY_RIGHT_SHIFT_RELEASED) {
					keyShift &= ~2;
				}
				if (i == keydata0 + KEY_CAPSLOCK) {
					keyLed ^= 4;
					fifo32_put(&keycmd, KEYBOARD_CMD_LED);
					fifo32_put(&keycmd, keyLed);
				}
				if (i == keydata0 + KEY_NUMLOCK) {
					keyLed ^= 2;
					fifo32_put(&keycmd, KEYBOARD_CMD_LED);
					fifo32_put(&keycmd, keyLed);
				}
				if (i == keydata0 + KEY_SCROLLLOCK) {
					keyLed ^= 1;
					fifo32_put(&keycmd, KEYBOARD_CMD_LED);
					fifo32_put(&keycmd, keyLed);
				}
				if (i == keydata0 + KEY_SEND_SUCCESS) {
					keycmdWait = -1;
				}
				if (i == keydata0 + KEY_SEND_FAIL) {
					waitKeyboardReady();
					io_out8(PORT_KEYBOARD, keycmdWait);
				}
				// Show the cursor again.
				boxfill8(sheetWindow->buf, sheetWindow->bxsize, cursorColor, cursorX, 28, cursorX + 7, 43);
				sheetRefresh(sheetWindow, cursorX, 28, cursorX + 8, 44);
			}
			// ------ Mouse ------ //
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
					if ((mdec.btn & 0x01) != 0) {
						sheetMove(sheetWindow, mx - 80, my - 8);
					}
				}
			}
			// ------ Cursor timer ------ //
			else if (i <= 1) {
				if (i != 0) {
					initTimer(timer, &fifo, 0);
					cursorColor = COL8_000000;
				}
				else {
					initTimer(timer, &fifo, 1);
					cursorColor = COL8_FFFFFF;
				}
				timerSetTimeOut(timer, 50);
				boxfill8(sheetWindow->buf, sheetWindow->bxsize, cursorColor, cursorX, 28, cursorX + 7, 43);
				sheetRefresh(sheetWindow, cursorX, 28, cursorX + 8, 44);
			}
		}
	}
}
