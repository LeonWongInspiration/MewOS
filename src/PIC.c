#include "PIC.h"

void initPIC(){
    io_out8(PIC0_IMR, 0xff);
	io_out8(PIC1_IMR, 0xff);

	io_out8(PIC0_ICW1, 0x11);
	io_out8(PIC0_ICW2, 0x20);
	io_out8(PIC0_ICW3, 1 << 2);
	io_out8(PIC0_ICW4, 0x01); 

	io_out8(PIC1_ICW1, 0x11);
	io_out8(PIC1_ICW2, 0x28);
	io_out8(PIC1_ICW3, 2);
	io_out8(PIC1_ICW4, 0x01);

	io_out8(PIC0_IMR, 0xfb);
	io_out8(PIC1_IMR, 0xff);
}

int *inthandler0c(int *esp){
	stackExceptionHandler(esp);
}

int *inthandler0d(int *esp){
	generalProtectedExceptionHandler(esp);
}

void inthandler20(int *esp){
	timerInterruptHandler(esp);
}

void inthandler21(int *esp){
	keyboardInterruptHandler(esp);
}

void inthandler2c(int *esp){
	mouseInterruptHandler(esp);
}

void inthandler27(int *esp){
	io_out8(PIC0_OCW2, 0x67);
}