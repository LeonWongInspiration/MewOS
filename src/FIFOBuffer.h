/** FIFO Data Handler
 * 
 * @author: Leon Wong
 * @build: 201809052346
 * @brief: This file defines a FIFO buffer and related functions.
 * @usage: This file should be both included and compiled.
 * 
 */ 

#ifndef FIFO_BUFFER_H
#define FIFO_BUFFER_H 1

#include "Task.h"
#include "Structs.h"

#include "include/stdio.h"

/// Flag of OVERRUN
const static int FIFO_BUFFER_OVERRUN = 0x0001;

/**
 * @brief: Init a FIFO8 Buffer
 * @param: FIFO8 *fifo: The buffer to init.
 * @param: int size: Size of the buffer.
 * @param: uchar *buf: The memory allocated for the buffer.
 * @warning: The size of the buffer should be the same with the real size of the buf array!
 */ 
void fifo8_init(struct FIFOBuffer8 *fifo, int size, unsigned char *buf);

/**
 * @brief: Put an element into the FIFO8 Buffer
 * @param: FIFO8 *fifo: The buffer to add an elem.
 * @param: uchar data: The data to be added.
 * @return: int, 0 if succeed, -1 otherwise.
 */ 
int fifo8_put(struct FIFOBuffer8 *fifo, unsigned char data);

/**
 * @brief: Get the first elem in the buffer.
 * @param: FIFO *fifo: The buffer.
 * @param: uchar *data: The uchar which will be set the value of the first elem.
 * @return: int, 0 if succeed, -1 otherwise.
 */ 
int fifo8_get(struct FIFOBuffer8 *fifo, unsigned char *data);

/**
 * @brief: Get how many elems are in a FIFO8 Buffer.
 * @param: FIFO8 *fifo: The FIFO8 Buffer.
 * @return: int, the number of total elems in the Buffer.
 */ 
int fifo8_status(struct FIFOBuffer8 *fifo);

/**
 * @brief: Init a FIFO32 Buffer
 * @param: FIFO32 *fifo: The buffer to init.
 * @param: int size: Size of the buffer.
 * @param: (int *)buf: The memory allocated for the buffer.
 * @param: (TASK *)task: We can assign a task for a buffer, thus the buffer will wake up the task when adding elems.
 * @warning: The size of the buffer should be the same with the real size of the buf array!
 * @warning: You can assign NULL for the task, thus the buffer won't wake up a task when adding elems.
 */ 
void fifo32_init(struct FIFOBuffer32 *fifo, int size, int *buf, struct TASK *task);

/**
 * @brief: Put an element into the FIFO32 Buffer
 * @param: FIFO32 *fifo: The buffer to add an elem.
 * @param: (int) data: The data to be added.
 * @return: int, 0 if succeed, -1 otherwise.
 */ 
int fifo32_put(struct FIFOBuffer32 *fifo, int data);

/**
 * @brief: Get the first elem in the buffer.
 * @param: FIFO32 *fifo: The buffer.
 * @param: (int *)data: The uchar which will be set the value of the first elem.
 * @return: int, 0 if succeed, -1 otherwise.
 */ 
int fifo32_get(struct FIFOBuffer32 *fifo, int *data);

/**
 * @brief: Get how many elems are in a FIFO32 Buffer.
 * @param: FIFO32 *fifo: The FIFO8 Buffer.
 * @return: int, the number of total elems in the Buffer.
 */ 
int fifo32_status(struct FIFOBuffer32 *fifo);

#endif
