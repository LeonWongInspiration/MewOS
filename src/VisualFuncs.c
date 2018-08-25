#include "VisualFuncs.h"

void init_palette(){
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:Black */
		0xff, 0x00, 0x00,	/*  1:Light red */
		0x00, 0xff, 0x00,	/*  2:Light green */
		0xff, 0xff, 0x00,	/*  3:Light yellow */
		0x00, 0x00, 0xff,	/*  4:Light blue */
		0xff, 0x00, 0xff,	/*  5:Light purple */
		0x00, 0xff, 0xff,	/*  6:Light light blue */
		0xff, 0xff, 0xff,	/*  7:White */
		0xc6, 0xc6, 0xc6,	/*  8:Light gray */
		0x84, 0x00, 0x00,	/*  9:Dark red */
		0x00, 0x84, 0x00,	/* 10:Dark green */
		0x84, 0x84, 0x00,	/* 11:Dark yellow */
		0x00, 0x00, 0x84,	/* 12:Dark cyan */
		0x84, 0x00, 0x84,	/* 13:Dark purple */
		0x00, 0x84, 0x84,	/* 14:Light dark blue */
		0x84, 0x84, 0x84	/* 15:Dark gray */
	};
	set_palette(0, 15, table_rgb);
}

void set_palette(int start, int end, unsigned char *colorInRGB){
	int i;
	int eflags; // Save the eflags
	eflags = io_load_eflags();
	io_cli(); // Disallow interrupts
	io_out8(0x03c8, start); // Claim the palette to video converter
	for (i = start; i<= end; ++i){
		io_out8(0x03c9, colorInRGB[0] / 4); // Tell video converter color in RGB
		io_out8(0x03c9, colorInRGB[1] / 4);
		io_out8(0x03c9, colorInRGB[2] / 4);
		colorInRGB += 3;
	}
	io_store_eflags(eflags); // Enable interrupts
}

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1){
	int x, y;

	int xmin, xmax, ymin, ymax;

	xmin = x0 < x1 ? x0 : x1;
	xmax = x0 < x1 ? x1 : x0;
	ymin = y0 < y1 ? y0 : y1;
	ymax = y0 < y1 ? y1 : y0;

	x0 = xmin;
	x1 = xmax;
	y0 = ymin;
	y1 = ymax;

	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = c;
	}
}

void init_screen(char *vram, int x, int y){
	boxfill8(vram, x, COL8_008484, 0, 0, x - 1, y - 29);
	boxfill8(vram, x, COL8_C6C6C6, 0, y - 28, x - 1, y - 28);
	boxfill8(vram, x, COL8_FFFFFF, 0, y - 27, x - 1, y - 27);
	boxfill8(vram, x, COL8_C6C6C6, 0, y - 26, x - 1, y - 1);

	boxfill8(vram, x, COL8_FFFFFF, 3, y - 24, 59, y - 24);
	boxfill8(vram, x, COL8_FFFFFF, 2, y - 24,  2, y - 4);
	boxfill8(vram, x, COL8_848484, 3, y - 4, 59, y - 4);
	boxfill8(vram, x, COL8_848484, 59, y - 23, 59, y - 5);
	boxfill8(vram, x, COL8_000000, 2, y - 3, 59, y - 3);
	boxfill8(vram, x, COL8_000000, 60, y - 24, 60, y - 3);

	boxfill8(vram, x, COL8_848484, x - 47, y - 24, x - 4, y - 24);
	boxfill8(vram, x, COL8_848484, x - 47, y - 23, x - 47, y - 4);
	boxfill8(vram, x, COL8_FFFFFF, x - 47, y - 3, x - 4, y - 3);
	boxfill8(vram, x, COL8_FFFFFF, x - 3, y - 24, x - 3, y -  3);
}

void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize){
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
}