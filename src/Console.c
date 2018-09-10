#include "Console.h"

#define __STOP while(1) {io_hlt();}

void consoleTask(SHEET *sheet, unsigned int totalMemory){
    TASK *task = getCurrentTask();
    if (task == NULL) {__STOP}
    MEMORY_FREE_TABLE *memman = (MEMORY_FREE_TABLE *)MEMORY_MANAGER_ADDR;
    int i;
    int *fat = (int *)allocMemoryForSize_Page(memman, 4 * 2880);
    CONSOLE cons;
    char cmdline[30];
    cons.sheet = sheet;
    cons.curX = 8;
    cons.curY = 28;
    cons.curColor = -1;
    task->cons = &cons;

    if (sheet != NULL) {
        cons.timer = allocTimer();
        if (cons.timer == NULL){__STOP}
        initTimer(cons.timer, &(task->fifo), 1);
        timerSetTimeOut(cons.timer, 50);
    }
    readFAT12(fat, (unsigned char *) (DISKIMG_ADDRESS + 0x000200));

    consolePutChar(&cons, '$', 1); // Show the prompt.

    while (1){
        io_cli();
        if (fifo32_status(&(task->fifo)) == 0) {
            setTaskSleep(task);
            io_sti();
        }
        else {
            fifo32_get(&(task->fifo), &i);
            io_sti();
            if (i <= 1) { // Now we have to change the color of the cursor to make it blink.
                if (i != 0) {
                    initTimer(cons.timer, &(task->fifo), 0);
                    if (cons.curColor >= 0) {
                        cons.curColor = COL8_FFFFFF;
                    }
                }
                else {
                    initTimer(cons.timer, &(task->fifo), 1);
                    if (cons.curColor >= 0) {
                        cons.curColor = COL8_000000;
                    }
                }
                timerSetTimeOut(cons.timer, 50);
            }
            if (i == 2) { // Switch to the terminal window initially.
                cons.curColor = COL8_FFFFFF;
            }
            if (i == 3) { // Terminal window now runs background, we should hide its cursor.
                if (sheet != NULL){
                    boxfill8(sheet->buf, sheet->bxsize, COL8_000000, cons.curX, cons.curY, cons.curX + 7, cons.curY + 15);
                }
                cons.curColor = -1;
            }
            if (i == 4) {
                cmdExit(&cons, fat);
            }
            if (i >= keydata0 && i < mousedata0) { // Handle data from keyboard.
                if (i == keydata0 + (int) '\b') { // Backspace
                    if (cons.curX > 16) { // We actually have characters to delete.
                        consolePutChar(&cons, ' ', 0);
                        cons.curX -= 8;
                    }
                }
                else if (i == keydata0 + (int) '\n') { // Enter
                    consolePutChar(&cons, ' ', 0);
                    // Read and run the cmd.
                    cmdline[cons.curX / 8 - 2] = '\0';
                    consoleNewLine(&cons);
                    consoleRunCmd(cmdline, &cons, fat, totalMemory);
                    if (sheet == NULL) {
                        cmdExit(&cons, fat);
                    }
                    //while (1) {io_hlt();}
                    // Show the next prompt.
                    consolePutChar(&cons, '$', 1);
                }
                else { // Other characters
                    if (cons.curX < 240) {
                        //__STOP
                        cmdline[cons.curX / 8 - 2] = i - keydata0;
                        consolePutChar(&cons, i - keydata0, 1);
                    }
                }
            }
            if (sheet != NULL){
                if (cons.curColor >= 0) {
                    boxfill8(sheet->buf, sheet->bxsize, cons.curColor, cons.curX, cons.curY, cons.curX + 7, cons.curY + 15);
                }
                sheetRefresh(sheet, cons.curX, cons.curY, cons.curX + 8, cons.curY + 16);
            }
        }
    }
}

void consolePutChar(CONSOLE *cons, int chr, char move) {
    char s[2];
    s[0] = chr;
    s[1] = '\0';
    if (s[0] == 0x09) { // \t
        while (1) {
            if (cons->sheet != NULL){
                putStringOnSheet(cons->sheet, cons->curX, cons->curY, COL8_FFFFFF, COL8_000000, " ", 1);
            }
            cons->curX += 8;
            if (cons->curX == 8 + 240) { // The line is full.
                consoleNewLine(cons);
            }
            if (((cons->curX - 8) & 0x1f) == 0) { // We have aligned for 4 chars.
                break;
            }
        }
    }
    else if (s[0] == 0x0a) { // \n
        consoleNewLine(cons);
    }
    else if (s[0] == 0x0d) { // \r
    }
    else {
        if (cons->sheet != NULL){
            putStringOnSheet(cons->sheet, cons->curX, cons->curY, COL8_FFFFFF, COL8_000000, s, 1);
        }
        if (move != 0) {
            cons->curX += 8;
            if (cons->curX == 8 + 240) {
                consoleNewLine(cons);
            }
        }
    }
}

void consoleNewLine(CONSOLE *cons){
    int x, y;
    SHEET *sht = cons->sheet;
    if (cons->curY < 28 + 112) { // There is no need to move the screen.
        cons->curY += 16;
    }
    else {
        if (sht != NULL){
            for (y = 28; y < 28 + 112; ++y) { // Move the lines up.
                for (x = 8; x < 8 + 240; ++x) {
                    sht->buf[x + y * sht->bxsize] = sht->buf[x + (y + 16) * sht->bxsize];
                }
            }
		    for (y = 28 + 112; y < 28 + 128; ++y) { // Remove the last line.
			    for (x = 8; x < 8 + 240; ++x) {
				    sht->buf[x + y * sht->bxsize] = COL8_000000;
			    }
		    }
            sheetRefresh(sht, 8, 28, 8 + 240, 28 + 128);
        }
    }
    cons->curX = 8;
}

void consoleWrite(CONSOLE *cons, char *s) {
    for (; *s != '\0'; ++s) {
        consolePutChar(cons, *s, 1);
    }
}

void consoleWriteLen(CONSOLE *cons, char *s, int l) {
    int i;
    for (i = 0; i < l; ++i) {
        consolePutChar(cons, s[i], 1);
    }
}

void consoleRunCmd(char *cmdline, CONSOLE *cons, int *fat, unsigned int totalMemory) {
    //while (1) {io_hlt();}
    if (strcmp(cmdline, "mem") == 0) {
        cmdMEM(cons, totalMemory);
    }
    else if (strcmp(cmdline, "clear") == 0) {
        cmdCLEAR(cons);
    }
    else if (strcmp(cmdline, "ls") == 0) {
        cmdLS(cons);
    }
    else if (strncmp(cmdline, "cat", 3) == 0 && (cmdline[3] == '\0' || cmdline[4] == '\0')) {
        consoleWrite(cons, "cat: requires one argument\n\n");
    }
    else if (strncmp(cmdline, "cat ", 4) == 0) {
        cmdCAT(cons, fat, cmdline);
    }
    else if (strncmp(cmdline, "start", 5) == 0 && (cmdline[5] == '\0' || cmdline[6] == '\0')) {
        consoleWrite(cons, "start: requires one argument\n\n");
    }
    else if (strncmp(cmdline, "start ", 6) == 0) {
        cmdStart(cons, cmdline, totalMemory);
    }
    else if (strncmp(cmdline, "call", 4) == 0 && (cmdline[4] == '\0' || cmdline[5] == '\0')) {
        consoleWrite(cons, "call: requires one argument\n\n");
    }
    else if (strncmp(cmdline, "call ", 5) == 0) {
        cmdCall(cons, cmdline, totalMemory);
    }
    else if (cmdline[0] != '\0') {
        if (cmdCALL(cons, fat, cmdline) == 0) {
            consoleWrite(cons, "Command not found.\n\n");
        }
    }
}

void cmdMEM(CONSOLE *cons, unsigned int totalMemory) {
    //while(1) {io_hlt();}
    MEMORY_FREE_TABLE *memman = (MEMORY_FREE_TABLE *) MEMORY_MANAGER_ADDR;
    char s[60];
    sprintf(s, "Total: %dMB\nFree: %d KB\n\n", totalMemory / (1024 * 1024), getAvailableMemorySpace(memman) / 1024);
    consoleWrite(cons, s);
}

void cmdCLEAR(CONSOLE *cons) {
    int x, y;
    SHEET *sht = cons->sheet;
    for (y = 28; y < 28 + 128; ++y) {
        for (x = 8; x < 8 + 240; ++x) {
            sht->buf[x + y * sht->bxsize] = COL8_000000;
        }
    }
    sheetRefresh(sht, 8, 28, 8 + 240, 28 + 128);
    cons->curY = 28;
}

void cmdLS(CONSOLE *cons) {
    FAT12 *fat = (FAT12 *) (DISKIMG_ADDRESS + 0x002600);
    int i, j;
    char s[30];
    for (i = 0; i < 224; ++i) {
        if (fat[i].name[0] == 0x00) { // Now we no longer have files after here.
            break;
        }
        if (fat[i].name[0] != 0xe5) { // If the file has been deleted, its name will start with 0xe5.
            if ((fat[i].type & 0x18) == 0) {
                sprintf(s, "filename.ext   %7d\n", fat[i].size);
                for (j = 0; j < 8; ++j) {
                    s[j] = fat[i].name[j];
                }
                s[9] = fat[i].ext[0];
                s[10] = fat[i].ext[1];
                s[11] = fat[i].ext[2];
                consoleWrite(cons, s);
            }
        }
    }
    consoleNewLine(cons);
}

void cmdCAT(CONSOLE *cons, int *fat, char *cmdline) {
    MEMORY_FREE_TABLE *memman = (MEMORY_FREE_TABLE *) MEMORY_MANAGER_ADDR;
    FAT12 *file = searchFile(cmdline + 4, (FAT12 *) (DISKIMG_ADDRESS + 0x002600), 224);
    char *p;
    if (file != NULL) {
        p = (char *) allocMemoryForSize_Page(memman, file->size);
        loadFile(file->clusterNo, file->size, p, fat, (char *) (DISKIMG_ADDRESS + 0x003e00));
        consoleWriteLen(cons, p, file->size);
        freeMemoryWithAddrAndSize_Page(memman, (int) p, file->size);
    }
    else {
        consoleWrite(cons, "no such file or directory.");
    }
    consoleNewLine(cons);
}

void cmdExit(CONSOLE *cons, int *fat) {
    MEMORY_FREE_TABLE *memman = (MEMORY_FREE_TABLE *) MEMORY_MANAGER_ADDR;
    TASK *task = getCurrentTask();
    SHEET_MANAGER *shtMan = (SHEET_MANAGER *) *((int *) 0x0fe4);
    FIFO32 *fifo = (FIFO32 *) *((int *) 0x0fec);
    if (cons->sheet != NULL) {
        cancelTimer(cons->timer);
    }
    freeMemoryWithAddrAndSize_Page(memman, (int) fat, 4 * 2880);
    io_cli();
    if (cons->sheet != NULL) {
        fifo32_put(fifo, cons->sheet - shtMan->sheets0 + 768); // Event 768 ~ 1023;
    }
    else {
        fifo32_put(fifo, task - taskManager->tasks + 1024); // Event 1024 ~ 2023;
    }
    io_sti();
    while (1) {
        setTaskSleep(task);
    }
}

void cmdStart(CONSOLE *cons, char *cmdline, int totalMemory) {
    SHEET_MANAGER *shtMan = (SHEET_MANAGER *) *((int *) 0x0fe4);
    SHEET *sht = openConsole(shtMan, totalMemory);
    FIFO32 *fifo = &(sht->task->fifo);
    int i;
    sheetMove(sht, 32, 4);
    setSheetHeight(sht, shtMan->top);
    for (i = 6; cmdline[i] != '\0'; ++i) {
        fifo32_put(fifo, cmdline[i] + keydata0);
    }
    fifo32_put(fifo, keydata0 + (int) '\n'); // Enter.
    consoleNewLine(cons);
}

void cmdCall(CONSOLE *cons, char *cmdline, int totalMemory) {
    TASK *task = openConsTask(0, totalMemory);
    FIFO32 *fifo = &(task->fifo);
    int i;
    for (i = 5; cmdline[i] != '\0'; ++i) {
        fifo32_put(fifo, cmdline[i] + keydata0);
    }
    fifo32_put(fifo, keydata0 + (int) '\n'); // Enter.
    consoleNewLine(cons);
}

int cmdCALL(CONSOLE *cons, int *fat, char *cmd) {
    MEMORY_FREE_TABLE *memman = (MEMORY_FREE_TABLE *) MEMORY_MANAGER_ADDR;
    FAT12 *file;
    SEGMENT_DESCRIPTOR *gdt = (SEGMENT_DESCRIPTOR *) GDTAddress;
    char name[18];
    char *p;
    char *q;
    TASK *task = getCurrentTask();
    int i, segsiz, datsiz, esp, dathrb;
    SHEET_MANAGER *sheetMan;
    SHEET *sht;

    // Handle the name of the file.
    for (i = 0; i < 13; ++i) {
        if (cmd[i] <= ' ') {
            break;
        }
        name[i] = cmd[i];
    }
    name[i] = '\0';

    // Search the file.
    file = searchFile(name, (FAT12 *) (DISKIMG_ADDRESS + 0x002600), 224);
    if (file == NULL && name[i - 1] != '.') {
        // Search again with ext name ".hrb"
        name[i] = '.';
        name[i + 1] = 'H';
        name[i + 2] = 'R';
        name[i + 3] = 'B';
        name[i + 4] = '\0';
        file = searchFile(name, (FAT12 *) (DISKIMG_ADDRESS + 0x002600), 224);
    }

    if (file != NULL) {
        // Found the file.
        p = (char *) allocMemoryForSize_Page(memman, file->size);
        loadFile(file->clusterNo, file->size, p, fat, (char *) (DISKIMG_ADDRESS + 0x003e00));
        if (file->size >= 36 && strncmp(p + 4, "Hari", 4) == 0 && *p == 0x00) {
            segsiz = *((int *) (p + 0x0000));
			esp    = *((int *) (p + 0x000c));
			datsiz = *((int *) (p + 0x0010));
			dathrb = *((int *) (p + 0x0014));
			q = (char *) allocMemoryForSize_Page(memman, segsiz);
            task->DSBase = (int) q;
            setGDT(gdt + 1000 + task->selector / 8, file->size - 1, (int) p, AR_CODE32_ER + 0x60);
            setGDT(gdt + 2000 + task->selector / 8, segsiz - 1, (int) q, AR_DATA32_RW + 0x60);
            for (i = 0; i < datsiz; i++) {
				q[esp + i] = p[dathrb + i];
			}
			start_app(0x1b, task->selector + 1000 * 8, esp, task->selector +  2000 * 8, &(task->tss.esp0));
            sheetMan = (SHEET_MANAGER *) *((int *) 0x0fe4);
            for (i = 0; i < MAX_SHEETS; ++i) {
                sht = &(sheetMan->sheets0[i]);
                if ((sht->flags & 0x11) == 0x11 && sht->task == task) {
                    sheetDestroy(sht);
                }
            }
            cancelAllTimers(&(task->fifo));
            freeMemoryWithAddrAndSize_Page(memman, (int) q, segsiz);
        }
        else {
            consoleWrite(cons, "HRB executable file format error.\n");
        }
        freeMemoryWithAddrAndSize_Page(memman, (int) p, file->size);
        consoleNewLine(cons);
        return 1;
    }
    return 0;
}

int *mew_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax){
    TASK *task = getCurrentTask();
    int DSBase = task->DSBase; // Save the data of DS register first.
    CONSOLE *cons = task->cons;
    SHEET_MANAGER *sheetMan = (SHEET_MANAGER *) *((int *) 0x0fe4);
    SHEET *sheet;
    int *reg = &eax + 1;
        /* reg[0] : EDI,   reg[1] : ESI,   reg[2] : EBP,   reg[3] : ESP */
		/* reg[4] : EBX,   reg[5] : EDX,   reg[6] : ECX,   reg[7] : EAX */
    int i;

    if (edx == 1) {
        consolePutChar(cons, eax & 0xff, 1); // ASM funcions can set EAX = char and call INT with edx = 1,
        // then MewOS will recognize EAX as a character and show it in the console.
    }
    else if (edx == 2) {
        consoleWrite(cons, (char *) ebx + DSBase); // ASM functions can set EBX = pointer to a C-string, and call INT with edx = 2,
        // then MewOS will recognize EBX as a C-string and show its contents in the console.
    }
    else if (edx == 3) {
        consoleWriteLen(cons, (char *) ebx + DSBase, ecx);
    }
    else if (edx == 4) {
        return &(task->tss.esp0);
    }
    else if (edx == 5) {
        sheet = allocASheetForWindow(sheetMan);
        sheet->task = task;
        sheet->flags |= 0x10; // Flag when window should be created by apps
        setSheetBuffer(sheet, (char *) ebx + DSBase, esi, edi, eax);
        make_window8((char *) ebx + DSBase, esi, edi, (char *) ecx + DSBase, 0);
        sheetMove(sheet, ((sheetMan->xsize - esi) / 2) & ~3, (sheetMan->ysize - edi) / 2);
        setSheetHeight(sheet, sheetMan->top);
        reg[7] = (int) sheet;
    }
    else if (edx == 6) {
        sheet = (SHEET *) (ebx & 0xfffffffe);
        putfonts8_asc(sheet->buf, sheet->bxsize, esi, edi, eax, (char *) ebp + DSBase);
        if ((ebx & 1) == 0){
            sheetRefresh(sheet, esi, edi, esi + ecx * 8, edi + 16);
        }
    }
    else if (edx == 7) {
        sheet = (SHEET *) (ebx & 0xfffffffe);
        boxfill8(sheet->buf, sheet->bxsize, ebp, eax, ecx, esi, edi);
        if ((ebx & 1) == 0){
            sheetRefresh(sheet, eax, ecx, esi + 1, edi + 1);
        }
    }
    else if (edx == 8) {
        memoryFreeTableInit((MEMORY_FREE_TABLE *) (ebx + DSBase));
        ecx &= 0xfffffff0;
        freeMemoryWithAddrAndSize((MEMORY_FREE_TABLE *) (ebx + DSBase), eax, ecx);
    }
    else if (edx == 9) {
        ecx = (ecx + 0x0f) & 0xfffffff0;
        reg[7] = allocMemoryForSize((MEMORY_FREE_TABLE *)(ebx + DSBase), ecx);
    }
    else if (edx == 10) {
        ecx = (ecx + 0x0f) & 0xfffffff0;
        freeMemoryWithAddrAndSize((MEMORY_FREE_TABLE *) (ebx + DSBase), eax, ecx);
    }
    else if (edx == 11) {
        sheet = (SHEET *) (ebx & 0xfffffffe);
        sheet->buf[sheet->bxsize * edi + esi] = eax;
        if ((ebx & 1) == 0) {
            sheetRefresh(sheet, esi, edi, esi + 1, edi + 1);
        }
    }
    else if (edx == 12) {
        sheet = (SHEET *)ebx;
        sheetRefresh(sheet, eax, ecx, esi, edi);
    }
    else if (edx == 13) {
        sheet = (SHEET *) (ebx & 0xfffffffe);
        mew_api_linewin(sheet, eax, ecx, esi, edi, ebp);
        if ((ebx & 1) == 0) {
            sheetRefresh(sheet, eax, ecx, esi + 1, edi + 1);
        }
    }
    else if (edx == 14) {
        sheetDestroy((SHEET *) ebx);
    }
    else if (edx == 15) {
        while (1){
            io_cli();
            if (fifo32_status(&task->fifo) == 0) {
                if (eax != 0) {
                    setTaskSleep(task);
                }
                else {
                    io_sti();
                    reg[7] = -1;
                    return 0;
                }
            }
            fifo32_get(&task->fifo, &i);
            io_sti();
            if (i <= 1) {
                initTimer(cons->timer, &task->fifo, 1);
                timerSetTimeOut(cons->timer, 50);
            }
            if (i == 2) {
                cons->curColor = COL8_FFFFFF;
            }
            if (i == 3) {
                cons->curColor = -1;
            }
            if (i >= keydata0) {
                reg[7] = i - keydata0;
                return 0;
            }
        }
    }
    else if (edx == 16) {
        reg[7] = (int) allocTimer();
        ((TIMER *) reg[7])->ownerStat = APPTIMER;
    }
    else if (edx == 17) {
        initTimer((TIMER *) ebx, &task->fifo, eax + keydata0);
    }
    else if (edx == 18) {
        timerSetTimeOut((TIMER *) ebx, eax);
    }
    else if (edx == 19) {
        freeTimer((TIMER *) ebx);
    }
    else if (edx == 20) {
        if (eax == 0) {
            i = io_in8(0x61);
            io_out8(0x61, i & 0x0d);
        }
        else {
            i = 1193180000 / eax;
            io_out8(0x43, 0xb6);
            io_out8(0x42, i & 0xff);
            io_out8(0x42, i >> 8);
            i = io_in8(0x61);
            io_out8(0x61, (i | 0x03) & 0x0f);
        }
    }
    return 0;
}

int *stackExceptionHandler(int *esp){
    TASK *task = getCurrentTask();
    CONSOLE *cons = task->cons;
    char s[30];
    consoleWrite(cons, "\n INT 0C :\n Stack Exception.\n");
    sprintf(s, "EIP = %08X\n", esp[11]);
    consoleWrite(cons, s);
    return &(task->tss.esp0);
}

int *generalProtectedExceptionHandler(int *esp){
    TASK *task  = getCurrentTask();
    CONSOLE *cons = task->cons;
    char s[30];
    consoleWrite(cons, "\nINT 0D :\n General Protected Exception.\n");
    sprintf(s, "EIP = %08X\n", esp[11]);
    consoleWrite(cons, s);
    return &(task->tss.esp0);
}

void mew_api_linewin(SHEET *sht, int x0, int y0, int x1, int y1, int col) {
    int i, x, y, len, dx, dy;

	dx = x1 - x0;
	dy = y1 - y0;
	x = x0 << 10;
	y = y0 << 10;
	if (dx < 0) {
		dx = - dx;
	}
	if (dy < 0) {
		dy = - dy;
	}
	if (dx >= dy) {
		len = dx + 1;
		if (x0 > x1) {
			dx = -1024;
		} else {
			dx =  1024;
		}
		if (y0 <= y1) {
			dy = ((y1 - y0 + 1) << 10) / len;
		} else {
			dy = ((y1 - y0 - 1) << 10) / len;
		}
	} 
    else {
		len = dy + 1;
		if (y0 > y1) {
			dy = -1024;
		} else {
			dy =  1024;
		}
		if (x0 <= x1) {
			dx = ((x1 - x0 + 1) << 10) / len;
		} else {
			dx = ((x1 - x0 - 1) << 10) / len;
		}
	}

	for (i = 0; i < len; i++) {
		sht->buf[(y >> 10) * sht->bxsize + (x >> 10)] = col;
		x += dx;
		y += dy;
	}
}

TASK *openConsTask(SHEET *sht, unsigned int totalMemory) {
    MEMORY_FREE_TABLE *memman = (MEMORY_FREE_TABLE *) MEMORY_MANAGER_ADDR;
    TASK *task = allocTask();
    int *consFifo = (int *) allocMemoryForSize_Page(memman, 128 * sizeof(int));
    task->consStack = allocMemoryForSize_Page(memman, 64 * 1024);
    task->tss.esp = task->consStack + 64 * 1024 - 12;
    task->tss.eip = (int) &consoleTask;
    task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;
    *((int *) (task->tss.esp + 4)) = (int) sht;
	*((int *) (task->tss.esp + 8)) = totalMemory;
    runTask(task, 2, 2);
    fifo32_init(&(task->fifo), 128, consFifo, task);
    return task;
}

SHEET *openConsole(SHEET_MANAGER *shtMan, unsigned int totalMemory) {
    MEMORY_FREE_TABLE *memman = (MEMORY_FREE_TABLE *) MEMORY_MANAGER_ADDR;
    SHEET *sht = allocASheetForWindow(shtMan);
    unsigned char *buf = (unsigned char *) allocMemoryForSize_Page(memman, 256 * 165);
    setSheetBuffer(sht, buf, 256, 165, -1);
    make_window8(buf, 256, 165, "console", 0);
    make_textbox8(sht, 8, 28, 240, 128, COL8_000000);
    sht->task = openConsTask(sht, totalMemory);
    sht->flags |= 0x20;
    return sht;
}

void closeConsTask(TASK *task) {
    MEMORY_FREE_TABLE *memman = (MEMORY_FREE_TABLE *) MEMORY_MANAGER_ADDR;
    setTaskSleep(task);
    freeMemoryWithAddrAndSize_Page(memman, task->consStack, 64 * 1024);
    freeMemoryWithAddrAndSize_Page(memman, (int) task->fifo.buf, 128 * sizeof(int));
    task->flags = TASK_FREE;
}

void closeConsole(SHEET *sht) {
    MEMORY_FREE_TABLE *memman = (MEMORY_FREE_TABLE *) MEMORY_MANAGER_ADDR;
    TASK *task = sht->task;
    freeMemoryWithAddrAndSize_Page(memman, (int) sht->buf, 256 * 165);
    sheetDestroy(sht);
    closeConsTask(task);
}
