#include "Sheet.h"

SHEET_MANAGER* initSheetManager(MEMORY_FREE_TABLE *memman, unsigned char *vram, int xsize, int ysize){
    SHEET_MANAGER *sheetManager;
    int i;
    sheetManager = allocMemoryForSize_Page(memman, sizeof(SHEET_MANAGER));
    if (sheetManager == NULL){
        return NULL;
    }
    sheetManager->vram = vram;
    sheetManager->xsize = xsize;
    sheetManager->ysize = ysize;
    sheetManager->top = -1; // No sheets
    for (i = 0; i < MAX_SHEETS; ++i){
        sheetManager->sheets0[i].flags = 0; // Not used
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

void setSheetHeight(SHEET_MANAGER *sheetManager, SHEET *sht, int height){
    int h, old = sht->height; // Save info before.

	// If the height is too high/low, adjust it.
	if (height > sheetManager->top + 1) {
		height = sheetManager->top + 1;
	}
	if (height < -1) {
		height = -1;
	}
	sht->height = height; // Assign the new height.

	// Reorder sheets[].
	if (old > height) {
		if (height >= 0) {
			for (h = old; h > height; h--) {
				sheetManager->sheets[h] = sheetManager->sheets[h - 1];
				sheetManager->sheets[h]->height = h;
			}
			sheetManager->sheets[height] = sht;
		} else {
			if (sheetManager->top > old) {
				for (h = old; h < sheetManager->top; h++) {
					sheetManager->sheets[h] = sheetManager->sheets[h + 1];
					sheetManager->sheets[h]->height = h;
				}
			}
			sheetManager->top--;
		}
		sheetRefreshSub(sheetManager, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize);
	} else if (old < height) {
		if (old >= 0) {
			for (h = old; h < height; h++) {
				sheetManager->sheets[h] = sheetManager->sheets[h + 1];
				sheetManager->sheets[h]->height = h;
			}
			sheetManager->sheets[height] = sht;
		} else {
			for (h = sheetManager->top; h >= height; h--) {
				sheetManager->sheets[h + 1] = sheetManager->sheets[h];
				sheetManager->sheets[h + 1]->height = h + 1;
			}
			sheetManager->sheets[height] = sht;
			sheetManager->top++;
        }
		sheetRefreshSub(sheetManager, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize);
	}
}

void sheetRefresh(SHEET_MANAGER *sheetManager, SHEET *sht, int bx0, int by0, int bx1, int by1){
    if (sht->height >= 0) { // If this sheet is visible
        sheetRefreshSub(sheetManager, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1);
    }
}

void sheetRefreshSub(SHEET_MANAGER *sheetManager, int vx0, int vy0, int vx1, int vy1){
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char *buf, c, *vram = sheetManager->vram;
	struct SHEET *sht;
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > sheetManager->xsize) { vx1 = sheetManager->xsize; }
	if (vy1 > sheetManager->ysize) { vy1 = sheetManager->ysize; }
	for (h = 0; h <= sheetManager->top; h++) {
		sht = sheetManager->sheets[h];
		buf = sht->buf;
		// If the sheet is outside, ignore those outside.
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) { bx0 = 0; }
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		for (by = by0; by < by1; by++) {
			vy = sht->vy0 + by;
			for (bx = bx0; bx < bx1; bx++) {
				vx = sht->vx0 + bx;
				c = buf[by * sht->bxsize + bx];
				if (c != sht->colorAndInvisibility) {
					vram[vy * sheetManager->xsize + vx] = c;
				}
			}
		}
	}
}
 
void sheetMove(SHEET_MANAGER *sheetManager, SHEET * sht, int vx0, int vy0){
    int old_vx0 = sht->vx0, old_vy0 = sht->vy0;
	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if (sht->height >= 0) { // If the sheet is currently visible.
		sheetRefreshSub(sheetManager, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize);
		sheetRefreshSub(sheetManager, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize);
	}
}

void sheetDestroy(SHEET_MANAGER *sheetManager, SHEET *sht){
    if (sht->height >= 0){
        setSheetHeight(sheetManager, sht, -1); // Set it invisible.
    }
    sht->flags = SHEET_FREE; // Set its flag as free.
}