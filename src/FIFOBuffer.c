#include "FIFOBuffer.h"

void fifo8_init(FIFO8 *fifo, int size, unsigned char *buf){
    fifo->size = size;
    fifo->buf = buf;
    fifo->free = size;
    fifo->flags = 0x0000;
    fifo->nextRead = 0;
    fifo->nextWrite = 0;
}

int fifo8_put(FIFO8 *fifo, unsigned char data){
    if (fifo->free == 0) {
		fifo->flags |= FIFO_BUFFER_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->nextWrite] = data;
	++(fifo->nextWrite);
	if (fifo->nextWrite == fifo->size) {
		fifo->nextWrite = 0;
	}
	--(fifo->free);
	return 0;
}

int fifo8_get(FIFO8 *fifo, unsigned char *data){
    if (fifo->free == fifo->size){
        return -1;
    }
    *data = fifo->buf[fifo->nextRead];
    ++(fifo->nextRead);
    if (fifo->nextRead == fifo->size){
        fifo->nextRead = 0;
    }
    ++(fifo->free);
    return 0;
}

int fifo8_status(FIFO8 *fifo){
    return fifo->size - fifo->free;
}

void fifo32_init(FIFO32 *fifo, int size, int *buf){
    fifo->size = size;
    fifo->buf = buf;
    fifo->free = size;
    fifo->flags = 0x0000;
    fifo->nextRead = 0;
    fifo->nextWrite = 0;
}

int fifo32_put(FIFO32 *fifo, int data){
    if (fifo->free == 0) {
		fifo->flags |= FIFO_BUFFER_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->nextWrite] = data;
	++(fifo->nextWrite);
	if (fifo->nextWrite == fifo->size) {
		fifo->nextWrite = 0;
	}
	--(fifo->free);
	return 0;
}

int fifo32_get(FIFO32 *fifo, int *data){
    if (fifo->free == fifo->size){
        return -1;
    }
    *data = fifo->buf[fifo->nextRead];
    ++(fifo->nextRead);
    if (fifo->nextRead == fifo->size){
        fifo->nextRead = 0;
    }
    ++(fifo->free);
    return 0;
}

int fifo32_status(FIFO32 *fifo){
    return fifo->size - fifo->free;
}
