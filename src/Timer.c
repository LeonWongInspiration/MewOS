#include "Timer.h"

TIMER_MANAGER timerManager;

void initPIT(){	
    int i;
	TIMER *t;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerManager.count = 0;
	for (i = 0; i < MAX_TIMER; i++) {
		timerManager.timers[i].flags = TIMER_FREE;
	}
	t = allocTimer();
	t->timeout = 0xffffffff;
	t->flags = TIMER_USING;
	t->next = NULL;
	timerManager.t0 = t;
	timerManager.next = 0xffffffff;
}

TIMER *allocTimer(){
    int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerManager.timers[i].flags == TIMER_FREE) {
			timerManager.timers[i].flags = TIMER_ALLOCED;
			timerManager.timers[i].ownerStat = SYSTIMER;
			return &timerManager.timers[i];
		}
	}
    return NULL;
}

void freeTimer(TIMER *timer){
    timer->flags = TIMER_FREE;
}

void initTimer(TIMER *timer, struct FIFOBuffer32 *fifo, int data){
    timer->fifo = fifo;
    timer->data = data;
}

void timerSetTimeOut(TIMER *timer, unsigned int timeout){
	int eflags;
	TIMER *t;
	TIMER *s;
	timer->timeout = timeout + timerManager.count;
	timer->flags = TIMER_USING;
	eflags = io_load_eflags();
	io_cli();
	t = timerManager.t0;
	if (timer->timeout <= t->timeout){
		// This timer should be the first one for its min timerout.
		timerManager.t0 = timer;
		timer->next = t;
		timerManager.next = timer->timeout;
		io_store_eflags(eflags);
		return;
	}
	// Insert the timer
	while (1){
		s = t;
		t = t->next;
		if (timer->timeout <= t->timeout) {
			s->next = timer;
			timer->next = t;
			io_store_eflags(eflags);
			return;
		}
	}
}

void timerInterruptHandler(int *esp){
	TIMER *timer;
	char taskSwitch = 0;
	io_out8(PIC0_OCW2, 0x60); // IRQ has been handled.
	++(timerManager.count);
	if (timerManager.next > timerManager.count){
		return;
	}
	timer = timerManager.t0;
	while (1){
		// For timers are all currently running, there's no need to store eflags.
		if (timer->timeout > timerManager.count){
			break;
		}
		// TIME OUT!
		timer->flags = TIMER_ALLOCED;
		if (timer != taskTimer){
			fifo32_put(timer->fifo, timer->data);
		}
		else {
			taskSwitch = 1;
		}
		timer = timer->next;
	}
	timerManager.t0 = timer;
	timerManager.next = timer->timeout;
	if (taskSwitch != 0){
		switchTask();
	}
}

int cancelTimer(TIMER *timer){
	int e;
	TIMER *t;
	e = io_load_eflags();
	io_cli();
	if (timer->flags == TIMER_USING) {
		if (timer == timerManager.t0) {
			t = timer->next;
			timerManager.t0 = t;
			timerManager.next = t->timeout;
		}
		else {
			t = timerManager.t0;
			while (1) {
				if (t->next == timer) {
					break;
				}
				t = t->next;
			}
			t->next = timer->next;
		}
		timer->flags = TIMER_ALLOCED;
		io_store_eflags(e);
		return 1;
	}
	io_store_eflags(e);
	return 0;
}

void cancelAllTimers(FIFO32 *fifo) {
	int e, i;
	TIMER *t;
	e = io_load_eflags();
	io_cli();
	for (i = 0; i < MAX_TIMER; ++i) {
		t = &timerManager.timers[i];
		if (t->flags != TIMER_FREE && t->ownerStat != SYSTIMER && t->fifo == fifo) {
			cancelTimer(t);
			freeTimer(t);
		}
	}
	io_store_eflags(e);
}
