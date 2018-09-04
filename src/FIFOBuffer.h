/** FIFO Data Handler
 * 
 * @author: Leon Wong
 * @build: 201809040844
 * @brief: This file defines a FIFO buffer and related functions.
 * @usage: This file should be both included and compiled.
 * 
 */ 

#if (!defined(FIFO_BUFFER_H))
#define FIFO_BUFFER_H 1

/// Flag of OVERRUN
const static int FIFO_BUFFER_OVERRUN = 0x0001;

/**
 * @brief: A FIFO Buffer including 8 bit data.
 */ 
typedef struct FIFOBuffer8{
    unsigned char *buf; /// Buffer
    int nextWrite; /// Index of next available byte to write.
    int nextRead; /// Index of next byte to read.
    int size; /// Size of the buffer.
    int free; /// Number of spaces available.
    int flags; /// Bitwise status indicator.
} FIFO8;

/**
 * @brief: A FIFO Buffer including 32bit data.
 */ 
typedef struct FIFOBuffer32{
    int *buf; /// Buffer
    int nextWrite; /// Index of next available byte to write.
    int nextRead; /// Index of next byte to read.
    int size; /// Size of the buffer.
    int free; /// Number of spaces available.
    int flags; /// Bitwise status indicator.
} FIFO32;

/**
 * @brief: Init a FIFO8 Buffer
 * @param: FIFO8 *fifo: The buffer to init.
 * @param: int size: Size of the buffer.
 * @param: uchar *buf: The memory allocated for the buffer.
 * @warning: The size of the buffer should be the same with the real size of the buf array!
 */ 
void fifo8_init(FIFO8 *fifo, int size, unsigned char *buf);

/**
 * @brief: Put an element into the FIFO8 Buffer
 * @param: FIFO8 *fifo: The buffer to add an elem.
 * @param: uchar data: The data to be added.
 * @return: int, 0 if succeed, -1 otherwise.
 */ 
int fifo8_put(FIFO8 *fifo, unsigned char data);

/**
 * @brief: Get the first elem in the buffer.
 * @param: FIFO *fifo: The buffer.
 * @param: uchar *data: The uchar which will be set the value of the first elem.
 * @return: int, 0 if succeed, -1 otherwise.
 */ 
int fifo8_get(FIFO8 *fifo, unsigned char *data);

/**
 * @brief: Get how many elems are in a FIFO8 Buffer.
 * @param: FIFO8 *fifo: The FIFO8 Buffer.
 * @return: int, the number of total elems in the Buffer.
 */ 
int fifo8_status(FIFO8 *fifo);

/**
 * @brief: Init a FIFO32 Buffer
 * @param: FIFO32 *fifo: The buffer to init.
 * @param: int size: Size of the buffer.
 * @param: (int *)buf: The memory allocated for the buffer.
 * @warning: The size of the buffer should be the same with the real size of the buf array!
 */ 
void fifo32_init(FIFO32 *fifo, int size, int *buf);

/**
 * @brief: Put an element into the FIFO32 Buffer
 * @param: FIFO32 *fifo: The buffer to add an elem.
 * @param: (int) data: The data to be added.
 * @return: int, 0 if succeed, -1 otherwise.
 */ 
int fifo32_put(FIFO32 *fifo, int data);

/**
 * @brief: Get the first elem in the buffer.
 * @param: FIFO32 *fifo: The buffer.
 * @param: (int *)data: The uchar which will be set the value of the first elem.
 * @return: int, 0 if succeed, -1 otherwise.
 */ 
int fifo32_get(FIFO32 *fifo, int *data);

/**
 * @brief: Get how many elems are in a FIFO32 Buffer.
 * @param: FIFO32 *fifo: The FIFO8 Buffer.
 * @return: int, the number of total elems in the Buffer.
 */ 
int fifo32_status(FIFO32 *fifo);

#endif
