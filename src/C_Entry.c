// Functions from nas
void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

// Functions in this file, declaration
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);

// Color RGB consts
const int COL8_000000 = 0;
const int COL8_FF0000 = 1;
const int COL8_00FF00 = 2;
const int COL8_FFFF00 = 3;
const int COL8_0000FF = 4;
const int COL8_FF00FF = 5;
const int COL8_00FFFF = 6;
const int COL8_FFFFFF = 7;
const int COL8_C6C6C6 = 8;
const int COL8_840000 = 9;
const int COL8_008400 = 10;
const int COL8_848400 = 11;
const int COL8_000084 = 12;
const int COL8_840084 = 13;
const int COL8_008484 = 14;
const int COL8_848484 = 15;

void MewOSMain(){
	int i;
	char *p;

	init_palette();

	p = (char *) 0xa0000;

	boxfill8(p, 320, COL8_FF0000,  20,  20, 120, 120);
	boxfill8(p, 320, COL8_00FF00,  70,  50, 170, 150);
	boxfill8(p, 320, COL8_0000FF, 120,  80, 220, 180);
	
	while (1){
		io_hlt();
	}
}

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
	return;
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
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = c;
	}
	return;
}