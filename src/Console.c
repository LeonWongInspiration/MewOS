#include "Console.h"

void consoleTask(SHEET *sheet, unsigned int totalMemory){
    TIMER *timer;
    TASK *task = getCurrentTask();
    MEMORY_FREE_TABLE *memman = (MEMORY_FREE_TABLE *)MEMORY_MANAGER_ADDR;
    int i;
    int fifobuf[128];
    int *fat = (int *)allocMemoryForSize_Page(memman, 4 * 2880);
    CONSOLE cons;
    char cmdline[30];
    cons.sheet = sheet;
    cons.curX = 8;
    cons.curY = 28;
    cons.curColor = -1;
    *((int *) 0x0fec) = (int) &cons;

    fifo32_init(&(task->fifo), 128, fifobuf, task);
    timer = allocTimer();
    initTimer(timer, &(task->fifo), 1);
    timerSetTimeOut(timer, 50); // Cursor will blink every 500 ms.
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
                    initTimer(timer, &(task->fifo), 0);
                    if (cons.curColor >= 0) {
                        cons.curColor = COL8_FFFFFF;
                    }
                }
                else {
                    initTimer(timer, &(task->fifo), 1);
                    if (cons.curColor >= 0) {
                        cons.curColor = COL8_000000;
                    }
                }
                timerSetTimeOut(timer, 50);
            }
            if (i == 2) { // Switch to the terminal window initially.
                cons.curColor = COL8_FFFFFF;
            }
            if (i == 3) { // Terminal window now runs background, we should hide its cursor.
                boxfill8(sheet->buf, sheet->bxsize, COL8_000000, cons.curX, cons.curY, cons.curX + 7, cons.curY + 15);
                cons.curColor = -1;
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
                    //while (1) {io_hlt();}
                    // Show the next prompt.
                    consolePutChar(&cons, '$', 1);
                }
                else { // Other characters
                    if (cons.curX < 240) {
                        cmdline[cons.curX / 8 - 2] = i - keydata0;
                        consolePutChar(&cons, i - keydata0, 1);
                    }
                }
            }
            if (cons.curColor >= 0) {
                boxfill8(sheet->buf, sheet->bxsize, cons.curColor, cons.curX, cons.curY, cons.curX + 7, cons.curY + 15);
            }
            sheetRefresh(sheet, cons.curX, cons.curY, cons.curX + 8, cons.curY + 16);
        }
    }
}

void consolePutChar(CONSOLE *cons, int chr, char move) {
    char s[2];
    s[0] = (char) chr;
    s[1] = '\0';
    if (s[0] == 0x09) { // \t
        while (1) {
            putStringOnSheet(cons->sheet, cons->curX, cons->curY, COL8_FFFFFF, COL8_000000, " ", 1);
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
        putStringOnSheet(cons->sheet, cons->curX, cons->curY, COL8_FFFFFF, COL8_000000, s, 1);
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
    cons->curX = 8;
}

void consoleWrite(CONSOLE *cons, char *s) {
    for (; s != '\0'; ++s) {
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
    else if (strncmp(cmdline, "cat ", 4) == 0) {
        cmdCAT(cons, fat, cmdline);
    }
    else if (cmdline[0] != '\0') {
        if (cmdCALL(cons, fat, cmdline) == 0) {
            consoleWrite(cons, "Command not found\n\n");
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
    FAT12 *file = searchFile(cmdline + 5, (FAT12 *) (DISKIMG_ADDRESS + 0x002600), 224);
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

int cmdCALL(CONSOLE *cons, int *fat, char *cmd) {
    MEMORY_FREE_TABLE *memman = (MEMORY_FREE_TABLE *) MEMORY_MANAGER_ADDR;
    FAT12 *file;
    SEGMENT_DESCRIPTOR *gdt = (SEGMENT_DESCRIPTOR *) GDTAddress;
    char name[18];
    char *p;
    int i;

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
        setGDT(gdt + 1003, file->size - 1, (int) p, AR_CODE32_ER);
        farcall(0, 1003 * 8);
        freeMemoryWithAddrAndSize_Page(memman, (int) p, file->size);
        consoleNewLine(cons);
        return 1;
    }
    return 0;
}

void mew_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax){
    CONSOLE *cons = (CONSOLE *) *((int *) 0x0fec);
    if (edx == 1) {
        consolePutChar(cons, eax & 0xff, 1); // ASM funcions can set EAX = char and call INT with edx = 1,
        // then MewOS will recognize EAX as a character and show it in the console.
    }
    else if (edx == 2) {
        consoleWrite(cons, (char *)ebx); // ASM functions can set EBX = pointer to a C-string, and call INT with edx = 2,
        // then MewOS will recognize EBX as a C-string and show its contents in the console.
    }
    else if (edx == 3) {
        consoleWriteLen(cons, (char *) ebx, ecx);
    }
}
