#include "Mouse.h"

FIFO32 mouseBuffer;
int mousedata0;

void init_mouse_cursor8(char *mouse, char bc){
    static char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
	int x, y;

	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 16 + x] = COL8_000000;
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 16 + x] = COL8_FFFFFF;
			}
			if (cursor[y][x] == '.') {
				mouse[y * 16 + x] = bc;
			}
		}
	}
}

void enableMouse(MOUSE_DECODER *mouseDecoder, int data0){
	mousedata0 = data0;
	waitKeyboardReady();
	io_out8(PORT_KEYBOARD_CMD, KEYBOARD_CMD_SENDTO_MOUSE);
	waitKeyboardReady();
	io_out8(PORT_KEYBOARD, MOUSECMD_ENABLE);
	// If mouse is ready, we'll get 0xfa from keyboard!
	mouseDecoder->phase = 0;
}

void mouseInterruptHandler(int *esp){
	int data;
	io_out8(PIC1_OCW2, 0x64);
	io_out8(PIC0_OCW2, 0x62);
	data = io_in8(PORT_KEYBOARD);
	fifo32_put(&mouseBuffer, data + mousedata0);
}

void initMouseBuffer(int *buf, int size){
    fifo32_init(&mouseBuffer, size, buf);
}

int mouseDecode(MOUSE_DECODER *mouseDecoder, unsigned char dat){
	if (mouseDecoder->phase == 0) {
		// Waiting for mouse to get ready (0xfa)
		if (dat == 0xfa) {
			mouseDecoder->phase = 1;
		}
		return 0;
	}
	if (mouseDecoder->phase == 1) {
		// Check if the first byte of the data is ligal.
		// It is OK if dont do this, but this can avoid bad data like when the mouse's link goes wrong.
		if ((dat & 0xc8) == 0x08) {
			mouseDecoder->decodeBuffer[0] = dat;
			mouseDecoder->phase = 2;
		}
		return 0;
	}
	if (mouseDecoder->phase == 2) {
		mouseDecoder->decodeBuffer[1] = dat;
		mouseDecoder->phase = 3;
		return 0;
	}
	if (mouseDecoder->phase == 3) {
		mouseDecoder->decodeBuffer[2] = dat;
		mouseDecoder->phase = 1;
		mouseDecoder->btn = mouseDecoder->decodeBuffer[0] & 0x07;
		mouseDecoder->x = mouseDecoder->decodeBuffer[1];
		mouseDecoder->y = mouseDecoder->decodeBuffer[2];
		if ((mouseDecoder->decodeBuffer[0] & 0x10) != 0) {
			mouseDecoder->x |= 0xffffff00;
		}
		if ((mouseDecoder->decodeBuffer[0] & 0x20) != 0) {
			mouseDecoder->y |= 0xffffff00;
		}
		mouseDecoder->y = - mouseDecoder->y; // The y axis of the mouse is just the opposite.
		return 1;
	}
	return -1;
}
