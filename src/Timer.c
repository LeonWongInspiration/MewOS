#include "Timer.h"

void initPIT(){
    io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
}

void timerInterruptHandler(int *esp){
    io_out8(PIC0_OCW2, 0x60);
}