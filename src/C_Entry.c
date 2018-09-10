/**
 * @brief: The Entry of C system files.
 * @author: Leon Wong
 * @build: 2018009081754
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
#include "Task.h"

#include "include/stdio.h"
#include "include/string.h"

static const char *version = "MewOS 0.5.0.0";

/**
 * ENTRY FUNCTION!
 */ 
void MewOSMain();

void MewOSMain(){
	BootInfo *binfo = (BootInfo *) ADR_BOOTINFO;
	SHEET_MANAGER *sheetManager;
	char s[40]; // For sprintf.
	FIFO32 fifo; // Unified FIFO buffer for IRQs.
	FIFO32 keycmd; // Keyboard commands.
	int fifobuf[128];
	int keycmdbuf[32];
	//TIMER *timer;
	int mx, my; // Position for mouse.
	int i;
	int new_mx = -1, new_my = 0, new_wx = 69589585, new_wy = 0;
	//int cursorX, cursorColor; // Position and color for cursor.
	// int count = 0;
	unsigned int totalMemory;
	MOUSE_DECODER mdec;
	MEMORY_FREE_TABLE *memman = (MEMORY_FREE_TABLE *) MEMORY_MANAGER_ADDR;
	unsigned char *bufferBackground;
	unsigned char bufferMouse[256];
	SHEET *sheetBackground;
	SHEET *sheetMouse;
	TASK *taskA, *task;
	int keyShift = 0;
	int keyLed = (binfo->leds >> 4) & 7;
	int keycmdWait = -1;
	int j, x, y;
	int mmx = -1, mmy = -1, mmx2 = 0; // Mouse Move positions.
	SHEET *sht = NULL, *key_win; // The window now receiving keys.

	// ------ Do system init ------ //
	initGDT();
	initIDT();
	initPIC();
	io_sti(); // Now GDT IDT has been inited, we can allow interrupts.
	fifo32_init(&fifo, 128, fifobuf, NULL);
	*((int *) 0x0fec) = (int) &fifo;
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
	freeMemoryWithAddrAndSize(memman, 0x00600000, totalMemory - 0x00600000); // What between the two parts are our system!

	// ------ Do sheets and tasks init ------ //
	init_palette();
	sheetManager = initSheetManager(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	if (sheetManager == NULL) {__STOP}
	taskA = initTask(memman);
	fifo.task = taskA;
	runTask(taskA, 1, 2);
	*((int *) 0x0fe4) = (int) sheetManager;

	// ------ Background sheet init ------ //
	sheetBackground = allocASheetForWindow(sheetManager);
	bufferBackground = (unsigned char*) allocMemoryForSize_Page(memman, binfo->scrnx * binfo->scrny);
	setSheetBuffer(sheetBackground, bufferBackground, binfo->scrnx, binfo->scrny, -1);
	init_screen(bufferBackground, binfo->scrnx, binfo->scrny);

	// ------ Console sheet init ------ //
	// sheetConsole = allocASheetForWindow(sheetManager);
	// bufferConsole = (unsigned char *)allocMemoryForSize_Page(memman, 256 * 165);
	// setSheetBuffer(sheetConsole, bufferConsole, 256, 165, -1);
	// make_window8(bufferConsole, 256, 165, "Console", 0);
	// make_textbox8(sheetConsole, 8, 28, 240, 128, COL8_000000);
	// taskConsole = allocTask();
	// taskConsole->tss.esp = allocMemoryForSize_Page(memman, 64 * 1024) + 64 * 1024 - 12;
	// taskConsole->tss.eip = (int) &consoleTask;
	// taskConsole->tss.es = 1 * 8;
	// taskConsole->tss.cs = 2 * 8;
	// taskConsole->tss.ss = 1 * 8;
	// taskConsole->tss.ds = 1 * 8;
	// taskConsole->tss.fs = 1 * 8;
	// taskConsole->tss.gs = 1 * 8;
	// *((int *) (taskConsole->tss.esp + 4)) = (int) sheetConsole; // "Variable passing"
	// *((int *) (taskConsole->tss.esp + 8)) = totalMemory;
	// runTask(taskConsole, 2, 2);
	key_win = openConsole(sheetManager, totalMemory);

	// ------ Window sheet init ------ //
	// sheetWindow = allocASheetForWindow(sheetManager);
	// bufferWindow = (unsigned char*) allocMemoryForSize_Page(memman, 160 * 52);
	// setSheetBuffer(sheetWindow, bufferWindow, 144, 52, -1);
	// make_window8(bufferWindow, 144, 52, "some task", 1);
	// make_textbox8(sheetWindow, 8, 28, 128, 16, COL8_FFFFFF);
	// cursorX = 8;
	// cursorColor = COL8_FFFFFF;
	// timer = allocTimer();
	// initTimer(timer, &fifo, 1);
	// timerSetTimeOut(timer, 50);

	// ------ Mouse sheet init ------ //
	sheetMouse = allocASheetForWindow(sheetManager);
	setSheetBuffer(sheetMouse, bufferMouse, 16, 16, 99);
	init_mouse_cursor8(bufferMouse, 99);
	mx = (binfo->scrnx - 16) / 2;
	my = (binfo->scrny - 28 - 16) / 2;
	
	// ------ Arrange initial windows ------ //
	sheetMove(sheetBackground, 0, 0);
	// sheetMove(sheetConsole, 32, 4);
	sheetMove(key_win, 32, 4);
	// sheetMove(sheetWindow, 64, 56);
	sheetMove(sheetMouse, mx, my);

	setSheetHeight(sheetBackground, 0);
	// setSheetHeight(sheetConsole, 1);
	setSheetHeight(key_win, 1);
	// setSheetHeight(sheetWindow, 2);
	setSheetHeight(sheetMouse, 2);
	keywin_on(key_win);

	// key_win = sheetWindow;
	// sheetConsole->task = taskConsole;
	// sheetConsole->flags |= 0x20; // 0x20 flag means the window is created by system.

	// sprintf(s, "(%3d, %3d)", mx, my);
	// putStringOnSheet(sheetBackground, 0, 0, COL8_FFFFFF, COL8_008484, s, 10);
	// sprintf(s, "memory %dMB   free : %dKB",
	// 		totalMemory / (1024 * 1024), getAvailableMemorySpace(memman) / 1024);
	// putStringOnSheet(sheetBackground, 0, 32, COL8_FFFFFF, COL8_008484, s, 40);

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
			// No interrupts now, just update frame and sleep.
			if (new_mx >= 0) {
				io_sti();
				sheetMove(sheetMouse, new_mx, new_my);
				new_mx = -1;
			}
			else if (new_wx != 69589585) {
				io_sti();
				sheetMove(sht, new_wx, new_wy);
				new_wx = 69589585;
			}
			else {
				setTaskSleep(taskA);
				io_sti();
			}
		}
		else {
			fifo32_get(&fifo, &i);
			io_sti();
			if (key_win != NULL && key_win->flags == SHEET_FREE){
				if (sheetManager->top == 1) {
					key_win = NULL;
				}
				else {
					key_win = sheetManager->sheets[sheetManager->top - 1];
					keywin_on(key_win);
				}
			}
			if (i >= keydata0 && i < mousedata0) {
				// sprintf(s, "%02X", i - keydata0);
				// putStringOnSheet(sheetBackground, 0, 16, COL8_FFFFFF, COL8_008484, s, 2);
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
				if (s[0] != 0 && key_win != NULL) {
					fifo32_put(&(key_win->task->fifo), s[0] + keydata0);
					// If the key is characters or symbols.
					// if (key_win == sheetWindow) {
					// 	// Send the key to taskA.
					// 	if (cursorX < 128) {
					// 		// Show a character and move the cursor.
					// 		s[1] = '\0';
					// 		putStringOnSheet(sheetWindow, cursorX, 28, COL8_000000, COL8_FFFFFF, s, 1);
					// 		cursorX += 8;
					// 	}
					// }
					// else {
					// 	fifo32_put(&(taskConsole->fifo), s[0] + keydata0);
					// }
				}
				// ------ Now deal with function keys ------ //
				// if (i == keydata0 + KEY_BACKSPACE) {
				// 	if (key_win == sheetWindow) {
				// 		if (cursorX > 8) {
				// 			putStringOnSheet(sheetWindow, cursorX, 28, COL8_000000, COL8_FFFFFF, " ", 1);
				// 			cursorX -= 8;
				// 		}
				// 	}
				// 	else {
				// 		fifo32_put(&(taskConsole->fifo), keydata0 + (int)'\b');
				// // 	}
				// }
				// if (i == keydata0 + KEY_ENTER) {
				// 	if (key_win != sheetWindow) {
				// 		fifo32_put(&(taskConsole->fifo), keydata0 + (int)'\n');
				// 	}
				// }
				if (i == keydata0 + KEY_TAB && key_win != NULL) {
					// cursorColor = keywin_off(key_win, sheetWindow, cursorColor, cursorX);
					// j = key_win->height - 1;
					// if (j == 0) {
					// 	j = sheetManager->top - 1;
					// }
					// key_win = sheetManager->sheets[j];
					// cursorColor = keywin_on(key_win, sheetWindow, cursorColor);
					keywin_off(key_win);
					j = key_win->height - 1;
					if (j == 0) {
						j = sheetManager->top - 1;
					}
					key_win = sheetManager->sheets[j];
					keywin_on(key_win);
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
				if (i == keydata0 + KEY_BACKSPACE && keyShift != 0 && key_win != NULL) {
					// cons = (CONSOLE *) *((int *) 0x0fec);
					// consoleWrite(cons, "\nSignal (Kill task)\n");
					// io_cli();
					// taskConsole->tss.eax = (int) &(taskConsole->tss.esp0);
					// taskConsole->tss.eip = (int) asm_kill_app;
					// io_sti();
					task = key_win->task;
					if (task != NULL && task->tss.ss0 != 0) {
						consoleWrite(task->cons, "\nSignal: Break\n");
						io_cli();
						task->tss.eax = (int) &(task->tss.esp0);
						task->tss.eip = (int) asm_kill_app;
						io_sti();
						runTask(task, -1, 0);
					}
				}
				if (i == keydata0 + KEY_F2 && keyShift != 0) {
					if (key_win != NULL) {
						keywin_off(key_win);
					}
					key_win = openConsole(sheetManager, totalMemory);
					sheetMove(key_win, 32 + sheetManager->top * 8, 4 + sheetManager->top * 8);
					setSheetHeight(key_win, sheetManager->top);
					keywin_on(key_win);
				}
				if (i == keydata0 + KEY_F11) {
					setSheetHeight(sheetManager->sheets[1], sheetManager->top - 1);
				}
				if (i == keydata0 + KEY_SEND_SUCCESS) {
					keycmdWait = -1;
				}
				if (i == keydata0 + KEY_SEND_FAIL) {
					waitKeyboardReady();
					io_out8(PORT_KEYBOARD, keycmdWait);
				}
				// Show the cursor again.
				// if (cursorColor >= 0){
				// 	boxfill8(sheetWindow->buf, sheetWindow->bxsize, cursorColor, cursorX, 28, cursorX + 7, 43);
				// }
				// sheetRefresh(sheetWindow, cursorX, 28, cursorX + 8, 44);
			}
			// ------ Mouse ------ //
			else if (i >= mousedata0 && i <= 767) {
				if (mouseDecode(&mdec, i - mousedata0) != 0) {
					// sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					// if ((mdec.btn & 0x01) != 0) {
					// 	s[1] = 'L';
					// }
					// if ((mdec.btn & 0x02) != 0) {
					// 	s[3] = 'R';
					// }
					// if ((mdec.btn & 0x04) != 0) {
					// 	s[2] = 'C';
					// }
					// putStringOnSheet(sheetBackground, 32, 16, COL8_FFFFFF, COL8_008484, s, 15);
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
					new_mx = mx;
					new_my = my;
					// sprintf(s, "(%3d, %3d)", mx, my);
					// putStringOnSheet(sheetBackground, 0, 0, COL8_FFFFFF, COL8_008484, s, 10);
					// sheetMove(sheetMouse, mx, my);
					if ((mdec.btn & 0x01) != 0) {
						if (mmx < 0) {
							for (j = sheetManager->top - 1; j > 0; --j) {
								sht = sheetManager->sheets[j];
								x = mx - sht->vx0;
								y = my - sht->vy0;
								if (x >= 0 && x < sht->bxsize && y >= 0 && y < sht->bysize) {
									if (sht->buf[y * sht->bxsize + x] != sht->colorAndInvisibility) {
										setSheetHeight(sht, sheetManager->top - 1);
										if (sht != key_win) {
											// cursorColor = keywin_off(key_win, sheetWindow, cursorColor, cursorX);
											// key_win = sht;
											// cursorColor = keywin_on(key_win, sheetWindow, cursorColor);
											keywin_off(key_win);
											key_win = sht;
											keywin_on(key_win);
										}
										if (x >= 3 && x < sht->bxsize - 3 && y >= 3 && y < 21) {
											mmx = mx; // Now mmx mmy are not 0, let's move the windows.
											mmy = my;
											mmx2 = sht->vx0;
											new_wy = sht->vy0;
										}
										if (x >= sht->bxsize - 21 && x < sht->bxsize - 5 && y >= 5 && y < 19) { // Close Button
											if ((sht->flags & 0x10) != 0){
												// cons = (CONSOLE *) *((int *) 0x0fec);
												// consoleWrite(cons, "\nSignal: Break.\n");
												// io_cli();
												// taskConsole->tss.eax = (int) &(taskConsole->tss.esp0);
												// taskConsole->tss.eip = (int) asm_kill_app;
												// io_sti();
												task = sht->task;
												consoleWrite(task->cons, "\nSignal: Break\n");
												io_cli();
												task->tss.eax = (int) &(task->tss.esp0);
												task->tss.eip = (int) asm_kill_app;
												io_sti();
												runTask(task, -1, 0);
											}
											else {
												task = sht->task;
												io_cli();
												fifo32_put(&(task->fifo), 4);
												io_sti();
											}
										}
										break;
									}
								}
							}
						}
						else {
							x = mx - mmx;
							y = my - mmy;
							// sheetMove(sht, sht->vx0 + x, sht->vy0 + y);
							new_wx = (mmx2 + x + 2) & ~3;
							new_wy += y;
							mmy = my;
						}
					}
					else {
						mmx = -1; // Do not use Mouse move mode.
						if (new_wx != 69589585) {
							sheetMove(sht, new_wx, new_wy);
							new_wx = 69589585;
						}
					}
				}
			}
			else if (i >= 768 && i < 1024) {
				closeConsole(sheetManager->sheets0 + (i - 768));
			}
			else if (i >= 1024 && i <= 2023) {
				closeConsTask(taskManager->tasks + (i - 1024));
			}
			// ------ Cursor timer ------ //
			// else if (i <= 1) {
			// 	// __STOP
			// 	if (i != 0) {
			// 		initTimer(timer, &fifo, 0);
			// 		if (cursorColor >= 0){
			// 			cursorColor = COL8_000000;
			// 		}
			// 	}
			// 	else {
			// 		initTimer(timer, &fifo, 1);
			// 		if (cursorColor >= 0){
			// 			cursorColor = COL8_FFFFFF;
			// 		}
			// 	}
			// 	timerSetTimeOut(timer, 50);
			// 	if (cursorColor >= 0){
			// 		boxfill8(sheetWindow->buf, sheetWindow->bxsize, cursorColor, cursorX, 28, cursorX + 7, 43);
			// 		sheetRefresh(sheetWindow, cursorX, 28, cursorX + 8, 44);
			// 	}
			// }
		}
	}
}
