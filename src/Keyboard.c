#include "Keyboard.h"

FIFO32 *keyboardFIFO;
int keydata0;

void keyboardInterruptHandler(int *esp){
    int data;
	io_out8(PIC0_OCW2, 0x61); // Tell PIC that IRQ01 has been handled (0x61 = 0x60 + IRQ1)
	data = io_in8(PORT_KEYBOARD);
    fifo32_put(keyboardFIFO, data + keydata0);
}

// void initKeyboardBuffer(int *buf, int size){
//     fifo32_init(&keyboardBuffer, size, buf);
// }

void waitKeyboardReady(){
    while (1){
        if ((io_in8(PORT_KEYBOARD_STAT) & KEYBOARD_STAT_NOTREADY) == 0)
            return;
    }
}

void initKeyboard(FIFO32 *fifo, int data0){
    keyboardFIFO = fifo;
    keydata0 = data0;
    waitKeyboardReady();
    io_out8(PORT_KEYBOARD_CMD, KEYBOARD_CMD_WRITE_MODE);
    waitKeyboardReady();
    io_out8(PORT_KEYBOARD, KEYBOARD_MODE_KBC);
}
