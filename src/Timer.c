#include "Timer.h"

TIMER_MANAGER timerManager;

void initPIT(){	
    int i;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerManager.count = 0;
	for (i = 0; i < MAX_TIMER; i++) {
		timerManager.timers[i].flags = TIMER_FREE;
	}
}

TIMER *allocTimer(){
    int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerManager.timers[i].flags == 0) {
			timerManager.timers[i].flags = TIMER_ALLOCED;
			return &timerManager.timers[i];
		}
	}
    return NULL;
}

void freeTimer(TIMER *timer){
    timer->flags = TIMER_FREE;
}

void initTimer(TIMER *timer, FIFO8 *fifo, unsigned int data){
    timer->fifo = fifo;
    timer->data = data;
}

void timerSetTimeOut(TIMER *timer, unsigned int timeout){
    timer->timeout = timeout;
    timer->flags = TIMER_USING;
}

void timerInterruptHandler(int *esp){
    int i;
	io_out8(PIC0_OCW2, 0x60); // IRQ has been handled
	timerManager.count++;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerManager.timers[i].flags == TIMER_USING) {
			timerManager.timers[i].timeout--;
			if (timerManager.timers[i].timeout == 0) {
				timerManager.timers[i].flags = TIMER_USING;
				fifo8_put(timerManager.timers[i].fifo, timerManager.timers[i].data);
			}
		}
	}
}