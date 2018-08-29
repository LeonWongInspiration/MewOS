#include "Keyboard.h"

/// Buffer for the keyboard.
FIFO8 keyboardBuffer;

void keyboardInterruptHandler(int *esp){
    unsigned char data;
	io_out8(PIC0_OCW2, 0x61); // Tell PIC that IRQ01 has been handled (0x61 = 0x60 + IRQ1)
	data = io_in8(PORT_KEYBOARD);
    fifo8_put(&keyboardBuffer, data);
}

void initKeyboardBuffer(unsigned char *buf, int size){
    fifo8_init(&keyboardBuffer, size, buf);
}

void waitKeyboardReady(){
    while (1){
        if (io_in8(PORT_KEYBOARD_STAT) & KEYBOARD_STAT_NOTREADY != 0)
        return;
    }
}

void initKeyboard(){
    waitKeyboardReady();
    io_out8(PORT_KEYBOARD_CMD, KEYBOARD_CMD_WRITE_MODE);
    waitKeyboardReady();
    io_out8(PORT_KEYBOARD, KEYBOARD_MODE_KBC);
}
