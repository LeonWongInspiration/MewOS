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
	unsigned char table2[216 * 3];
	int r, g, b;
	set_palette(0, 15, table_rgb);
	for (b = 0; b < 6; ++b) {
		for (g = 0; g < 6; ++g) {
			for (r = 0; r < 6; ++r) {
				table2[(r + g * 6 + b * 36) * 3 + 0] = r * 51;
				table2[(r + g * 6 + b * 36) * 3 + 1] = g * 51;
				table2[(r + g * 6 + b * 36) * 3 + 2] = b * 51;
			}
		}
	}
	set_palette(16, 231, table2);
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
    MEMORY_FREE_TABLE *memman = (MEMORY_FREE_TABLE *)MEMORY_MANAGER_ADDR;
    int *fat = (int *)allocMemoryForSize_Page(memman, 4 * 2880);
    readFAT12(fat, (unsigned char *) (DISKIMG_ADDRESS + 0x000200));
	FAT12 *wallpp = searchFile("wallpp.mjp", (FAT12 *) (DISKIMG_ADDRESS + 0x002600), 224);
	char *p;
	if (wallpp != NULL) {
		p = (char *)allocMemoryForSize_Page(memman, wallpp->size);
		loadFile(wallpp->clusterNo, wallpp->size, p, fat, (char *) (DISKIMG_ADDRESS + 0x003e00));
	}
	char wallPaper[320][240];
	int a, b, c = 0;
	for (a = 0; a < 320; ++a) {
		for (b = 0; b < 240; ++b) {
			if (p[c] == 0x0d) {
				++c;
			}
			if (p[c] == 0x0a) {
				++c;
			}
			if (p[c] >= '0' && p[c] <= '9') {
				wallPaper[a][b] = p[c] - '0';
			}
			else if (p[c] >= 'A' && p[c] <= 'F') {
				wallPaper[a][b] = p[c] - 'A' + 0xA;
			}
			++c;
		}
	}
	freeMemoryWithAddrAndSize_Page(memman, (unsigned int) p, wallpp->size);

	int i, j;
	if (x == 320) {
		for (i = 0; i < 320; ++i) {
			for (j = 0; j < 240; ++j) {
				*vram = wallPaper[i][j];
				++vram;
			}
		}
		return;
	}
	if (x == 640 && y == 480) {
		for (i = 0; i < 640; ++i) {
			for (j = 0; j < 480; ++j) {
				*vram = wallPaper[i / 2][j / 2];
				++vram;
			}
		}
		return;
	}
	if (x == 1280 && y == 1024) {
		for (i = 0; i < 1280; ++i) {
			for (j = 0; j < 960; ++j) {
				*vram = wallPaper[i / 4][j / 4];
				++vram;
			}
		}
		for (i = 0; i < 1280 * 1024 - 1280 * 960; ++i){
			vram[i] = COL8_000000;
		}
		return;
	}
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