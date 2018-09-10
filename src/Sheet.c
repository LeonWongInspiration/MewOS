#include "Sheet.h"

SHEET_MANAGER* initSheetManager(MEMORY_FREE_TABLE *memman, unsigned char *vram, int xsize, int ysize){
    SHEET_MANAGER *sheetManager;
    int i;
    // Alloc memory for sheet manager.
    sheetManager = (SHEET_MANAGER *)allocMemoryForSize_Page(memman, sizeof(SHEET_MANAGER));
    if (sheetManager == NULL){
        return NULL;
    }
    // Alloc memory for the map.
    sheetManager->map = (unsigned char *)allocMemoryForSize_Page(memman, xsize * ysize);
    if (sheetManager->map == NULL){
        // If map cannot get enough memory space.
        // Destroy the memory allocated for sheet manager.
        freeMemoryWithAddrAndSize_Page(memman, (unsigned)sheetManager, sizeof(SHEET_MANAGER));
        return NULL;
    }
    sheetManager->vram = vram;
    sheetManager->xsize = xsize;
    sheetManager->ysize = ysize;
    sheetManager->top = -1; // No sheets
    for (i = 0; i < MAX_SHEETS; ++i){
        sheetManager->sheets0[i].flags = 0; // Not used
        sheetManager->sheets0[i].ctl = sheetManager;
    }
    return sheetManager;
}

SHEET *allocASheetForWindow(SHEET_MANAGER *sheetManager){
    SHEET *sheet;
    int i;
    for (i = 0; i < MAX_SHEETS; ++i){
        if (sheetManager->sheets0[i].flags == SHEET_FREE) {
            sheet = sheetManager->sheets0 + i;
            sheet->flags = SHEET_IN_USE;
            sheet->height = -1;
			sheet->task = NULL; // In default we do not add a task for the sheet.
            return sheet; // Succeed in making a new sheet.
        }
    }
    return NULL; // Failed.
}

void setSheetBuffer(SHEET *sht, unsigned char *buf, int xsize, int ysize, int colorAndInvisibility){
	sht->buf = buf;
	sht->bxsize = xsize;
	sht->bysize = ysize;
	sht->colorAndInvisibility = colorAndInvisibility;
}

void setSheetHeight(SHEET *sht, int height){
    SHEET_MANAGER *sheetManager = sht->ctl;
	int h, old = sht->height; // Save old info

	// Adjust height when its too high or low
	if (height > sheetManager->top + 1) {
		height = sheetManager->top + 1;
	}
	if (height < -1) {
		height = -1;
	}
	sht->height = height; // Assign new height

	// Reorder sheets[]
	if (old > height) {
		if (height >= 0) {
			for (h = old; h > height; h--) {
				sheetManager->sheets[h] = sheetManager->sheets[h - 1];
				sheetManager->sheets[h]->height = h;
			}
			sheetManager->sheets[height] = sht;
			refreshSheetMap(sheetManager, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1);
			sheetRefreshSub(sheetManager, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1, old);
		}
		else {
			if (sheetManager->top > old) {
				for (h = old; h < sheetManager->top; h++) {
					sheetManager->sheets[h] = sheetManager->sheets[h + 1];
					sheetManager->sheets[h]->height = h;
				}
			}
			sheetManager->top--;
			refreshSheetMap(sheetManager, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0);
			sheetRefreshSub(sheetManager, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0, old - 1);
		}
	}
	else if (old < height) {
		if (old >= 0) {
			for (h = old; h < height; h++) {
				sheetManager->sheets[h] = sheetManager->sheets[h + 1];
				sheetManager->sheets[h]->height = h;
			}
			sheetManager->sheets[height] = sht;
		}
		else {
			for (h = sheetManager->top; h >= height; h--) {
				sheetManager->sheets[h + 1] = sheetManager->sheets[h];
				sheetManager->sheets[h + 1]->height = h + 1;
			}
			sheetManager->sheets[height] = sht;
			sheetManager->top++; 
		}
		refreshSheetMap(sheetManager, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height);
		sheetRefreshSub(sheetManager, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height, height);
	}
}

void sheetRefresh(SHEET *sht, int bx0, int by0, int bx1, int by1){
    if (sht->height >= 0) { // If this sheet is visible
        sheetRefreshSub(sht->ctl, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1, sht->height, sht->height);
    }
}

void sheetRefreshSub(SHEET_MANAGER *sheetManager, int vx0, int vy0, int vx1, int vy1, int leastRefHeight, int maxRefHeight){
	int h, bx, by, vx, vy, bx0, by0, bx1, by1, bx2, sid4, i, i1, *p, *q, *r;
	unsigned char *buf, *vram = sheetManager->vram, *map = sheetManager->map, sid;
	struct SHEET *sht;
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > sheetManager->xsize) { vx1 = sheetManager->xsize; }
	if (vy1 > sheetManager->ysize) { vy1 = sheetManager->ysize; }
	for (h = leastRefHeight; h <= maxRefHeight; h++) {
		sht = sheetManager->sheets[h];
		buf = sht->buf;
		sid = sht - sheetManager->sheets0;
		// Ignore those parts outside
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) { bx0 = 0; }
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		if ((sht->vx0 & 3) == 0) {
			i = (bx0 + 3) / 4;
			i1 = bx1 / 4;
			i1 = i1 - i;
			sid4 = sid | sid << 8 || sid << 16 | sid << 24;
			for (by = by0; by < by1; ++by) {
				vy = sht->vy0 + by;
				for (bx = bx0; bx < bx1 && (bx & 3) != 0; ++bx) {
					vx = sht->vx0 + bx;
					if (map[vy * sheetManager->xsize + vx] == sid) {
						vram[vy * sheetManager->xsize + vx] = buf[by * sht->bxsize + bx];
					}
				}
				vx = sht->vx0 + bx;
				p = (int *) &map[vy * sheetManager->xsize + vx];
				q = (int *) &vram[vy * sheetManager->xsize + vx];
				r = (int *) &buf[by * sht->bxsize + bx];
				for (i = 0; i < i1; ++i) {
					if (p[i] == sid4) {
						q[i] = r[i];
					}
					else {
						bx2 = bx + i * 4;
						vx = sht->vx0 + bx2;
						if (map[vy * sheetManager->xsize + vx + 0] == sid) {
							vram[vy * sheetManager->xsize + vx + 0] = buf[by * sht->bxsize + bx2 + 0];
						}
						if (map[vy * sheetManager->xsize + vx + 1] == sid) {
							vram[vy * sheetManager->xsize + vx + 1] = buf[by * sht->bxsize + bx2 + 1];
						}
						if (map[vy * sheetManager->xsize + vx + 2] == sid) {
							vram[vy * sheetManager->xsize + vx + 2] = buf[by * sht->bxsize + bx2 + 2];
						}
						if (map[vy * sheetManager->xsize + vx + 3] == sid) {
							vram[vy * sheetManager->xsize + vx + 3] = buf[by * sht->bxsize + bx2 + 3];
						}
					}
				}
				for (bx += i1 * 4; bx < bx1; bx++) {				/* ���̒[����1�o�C�g���� */
					vx = sht->vx0 + bx;
					if (map[vy * sheetManager->xsize + vx] == sid) {
						vram[vy * sheetManager->xsize + vx] = buf[by * sht->bxsize + bx];
					}
				}
			}
		}
		else {
			for (by = by0; by < by1; by++) {
				vy = sht->vy0 + by;
				for (bx = bx0; bx < bx1; bx++) {
					vx = sht->vx0 + bx;
					if (map[vy * sheetManager->xsize + vx] == sid) {
						vram[vy * sheetManager->xsize + vx] = buf[by * sht->bxsize + bx];
					}
				}
			}
		}
	}
}
 
void sheetMove(SHEET *sht, int vx0, int vy0){
	SHEET_MANAGER *ctl = sht->ctl;
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;
	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if (sht->height >= 0) { // Update those showing
		refreshSheetMap(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0);
		refreshSheetMap(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height);
		sheetRefreshSub(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0, sht->height - 1);
		sheetRefreshSub(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height, sht->height);
	}
}

void sheetDestroy(SHEET *sht){
    if (sht->height >= 0){
        setSheetHeight(sht, -1); // Set it invisible.
    }
    sht->flags = SHEET_FREE; // Set its flag as free.
}

void refreshSheetMap(SHEET_MANAGER *sheetManager, int vx0, int vy0, int vx1, int vy1, int leastRefHeight){
    int h, bx, by, vx, vy, bx0, by0, bx1, by1, sid4, *p;
	unsigned char *buf, sid, *map = sheetManager->map;
	struct SHEET *sht;
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > sheetManager->xsize) { vx1 = sheetManager->xsize; }
	if (vy1 > sheetManager->ysize) { vy1 = sheetManager->ysize; }
	for (h = leastRefHeight; h <= sheetManager->top; h++) {
		sht = sheetManager->sheets[h];
		sid = sht - sheetManager->sheets0;
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) { bx0 = 0; }
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		if (sht->colorAndInvisibility == -1) {
			if ((sht->vx0 & 3) == 0 && (bx0 & 3) == 0 && (bx1 & 3) == 0) {
				bx1 = (bx1 - bx0) / 4;
				sid4 = sid | sid << 8 | sid << 16 | sid << 24;
				for (by = by0; by < by1; by++) {
					vy = sht->vy0 + by;
					vx = sht->vx0 + bx0;
					p = (int *) &map[vy * sheetManager->xsize + vx];
					for (bx = 0; bx < bx1; bx++) {
						p[bx] = sid4;
					}
				}
			}
			else {
				for (by = by0; by < by1; by++) {
					vy = sht->vy0 + by;
					for (bx = bx0; bx < bx1; bx++) {
						vx = sht->vx0 + bx;
						map[vy * sheetManager->xsize + vx] = sid;
					}
				}
			}
		}
		else {
			for (by = by0; by < by1; by++) {
				vy = sht->vy0 + by;
				for (bx = bx0; bx < bx1; bx++) {
					vx = sht->vx0 + bx;
					if (buf[by * sht->bxsize + bx] != sht->colorAndInvisibility) {
						map[vy * sheetManager->xsize + vx] = sid;
					}
				}
			}
		}
	}
}

void putStringOnSheet(SHEET *sht, int x, int y, int c, int b, char *s, int l){
	boxfill8(sht->buf, sht->bxsize, b, x, y, x + l * 8 - 1, y + 15);
	putfonts8_asc(sht->buf, sht->bxsize, x, y, c, s);
	sheetRefresh(sht, x, y, x + l * 8, y + 16);
}
