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
