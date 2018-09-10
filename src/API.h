/** MewOS System APIs
 * 
 * @author: Leon Wong  
 * @build: 201809092216
 * @brief: This file includes claims for MewOS system APIs provided for third-party apps.
 * @usage: This file should only be included.
 * 
 */ 

/**
 * @brief: Put a char to a console.
 * @param: (int)c: The char to put in a console.
 */ 
void api_putchar(int c);

/**
 * @brief: Put a C-style string into a console.
 * @param: (char *)s: The string to put in a console.
 */ 
void api_putstr0(char *s);

/**
 * @brief: App ends with this function. = sys.exit(0).
 */ 
void api_end();

/**
 * @brief: Create a window.
 * @param: (char *)buf: The buffer of the window.
 * @param: (int)xsiz: The x size of the window.
 * @param: (int)ysiz: The y size of the window.
 * @param: (int)colorAndInvisibility: Just as its name says.
 * @param: (char *)title: Title of the window.
 * @return: (int) The address of the window.
 */ 
int api_openwin(char *buf, int xsiz, int ysiz, int colorAndInvisibility, char *title);

/**
 * @brief: Put a string onto a window.
 * @param: (int)win: The address of the window.
 * @param: (int)x: The x position of the string.
 * @param: (int)y: The y position of the string.
 * @param: (int)col: The color of the window.
 * @param: (int)len: Length of the string.
 * @param: (char *)str: The C-style string.
 */ 
void api_putstrwin(int win, int x, int y, int col, int len, char *str);

/**
 * @brief: Draw boxes on a window.
 * @param: (int)win: The address of the window.
 * @param: (int)x0: The x0 position of the box.
 * @param: (int)y0: The y0 position of the box.
 * @param: (int)x1: The x1 position of the box.
 * @param: (int)y1: The y1 position of the box.
 * @param: (int)col: The color of the box.
 */ 
void api_boxfilwin(int win, int x0, int y0, int x1, int y1, int col);

/**
 * @brief: Init memory management for malloc.
 */ 
void api_initmalloc();

/**
 * @brief: Malloc function.
 * @param: (int)size: The size of the memory to assign.
 * @return: (char *): The address of memory assigned.
 */ 
char *api_malloc(int size);

/**
 * @brief: Free function.
 * @param: (char *)addr: The address of the memory to free.
 * @param: (int)size: The size of the memory to free.
 */ 
void api_free(char *addr, int size);

/**
 * @brief: Draw a point on a window.
 * @param: (int)win: Address of the window.
 * @param: (int)x: The x position of the point.
 * @param: (int)y: The y position of the point.
 * @param: (int)col: The color of the point.
 */ 
void api_point(int win, int x, int y, int col);

/**
 * @brief: Refresh a window.
 * @param: (int)win: Address of the window.
 * @param: (int)x0: The x0 position of the area to refresh.
 * @param: (int)y0: The y0 position of the area to refresh.
 * @param: (int)x1: The x1 position of the area to refresh.
 * @param: (int)y1: The y1 position of the area to refresh.
 */ 
void api_refreshwin(int win, int x0, int y0, int x1, int y1);

/**
 * @brief: Draw a line on a window.
 * @param: (int)win: The address of the window.
 * @param: (int)x0: The x0 position of the line.
 * @param: (int)y0: The y0 position of the line.
 * @param: (int)x1: The x1 position of the line.
 * @param: (int)y1: The y1 position of the line.
 * @param: (int)col: The color of the line.
 */ 
void api_linewin(int win, int x0, int y0, int x1, int y1, int col);

/**
 * @brief: Close a window.
 * @param: (int)win: Address of the window.
 */ 
void api_closewin(int win);

/**
 * @brief: Wait and get a key inout from the keyboard.
 * @param: (int)mode: The mode of the key you want.
 * @return: (int): The key.
 */ 
int api_getkey(int mode);

/**
 * @brief: Assign a timer for apps.
 * @return: (int)Address of the timer.
 */ 
int api_alloctimer();

/**
 * @brief: Init a timer.
 * @param: (int)timer: The address of the timer.
 * @param: (int)data: The data of the timer.
 */ 
void api_inittimer(int timer, int data);

/**
 * @brief: Set timeout for a timer.
 * @param: (int)timer: Address of the timer.
 * @param: (int)time: Timeout of the timer.
 */ 
void api_settimer(int timer, int time);

/**
 * @brief: Release a timer.
 * @param: (int)timer: Address of the timer.
 */ 
void api_freetimer(int timer);
