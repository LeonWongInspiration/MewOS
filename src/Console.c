#include "Console.h"

void consoleTask(SHEET *sheet){
    TIMER *timer;
    TASK *task = getCurrentTask();
    int i;
    int fifobuf[128];
    int cursorX = 16; // The initial X of cursor.
    int cursorColor = COL8_000000;
    char s[2];

    fifo32_init(&(task->fifo), 128, fifobuf, task);
    timer = allocTimer();
    initTimer(timer, &(task->fifo), 1);
    timerSetTimeOut(timer, 50); // Cursor will blink every 500 ms.

    putStringOnSheet(sheet, 8, 28, COL8_FFFFFF, COL8_000000, "$", 1); // Show the prompt.

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
                    cursorColor = COL8_FFFFFF;
                }
                else {
                    initTimer(timer, &(task->fifo), 1);
                    cursorColor = COL8_000000;
                }
                timerSetTimeOut(timer, 50);
            }
            if (i >= keydata0 && i < mousedata0) { // Handle data from keyboard.
                if (i == keydata0 + 8) { // Backspace
                    if (cursorX > 16) { // We actually have characters to delete.
                        putStringOnSheet(sheet, cursorX, 28, COL8_FFFFFF, COL8_000000, " ", 1);
                        cursorX -= 8;
                    }
                }
                else { // Other characters
                    if (cursorX < 240) {
                        s[0] = i - keydata0;
                        s[1] = '\0';
                        putStringOnSheet(sheet, cursorX, 28, COL8_FFFFFF, COL8_000000, s, 1);
                        cursorX += 8;
                    }
                }
            }
            boxfill8(sheet->buf, sheet->bxsize, cursorColor, cursorX, 28, cursorX + 7, 43);
            sheetRefresh(sheet, cursorX, 28, cursorX + 8, 44);
        }
    }
}
