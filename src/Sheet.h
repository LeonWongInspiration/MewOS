/** Window Sheets Management
 * 
 * @author: Leon Wong
 * @build: 201809102310 FINAL
 * @brief: This files contains some functions about window management.
 * @usage: This files shoule be both included and compiled.
 * 
 */ 

#ifndef SHEET_H
#define SHEET_H 1

#include "Memorie.h"
#include "Fonts.h"
#include "VisualFuncs.h"
#include "Structs.h"

#include "include/stdio.h"

/// The FLAGS for sheets:
const static int SHEET_FREE = 0;
const static int SHEET_IN_USE = 1;

/**
 * @brief: Init a SheetManager and return it.
 * @param: (MEMORY*) memman: Memory Manager.
 * @param: (uchar *) vram: Address of VRAM.
 * @param: (int) xsize: X resolution.
 * @param: (int) ysize: Y resolution.
 */ 
struct SHEET_MANAGER* initSheetManager(struct MEMORY_FREE_TABLE *memman, unsigned char *vram, int xsize, int ysize);

/**
 * @brief: Alloc a sheet for a window.
 * @param: (SHEET_MANAGER *) sheetManager: The sheet manager pointer.
 * @return (SHEET *) A sheet assigned for the window.
 * @warning: This may return a NULL when there's no available sheets!
 */
struct SHEET *allocASheetForWindow(struct SHEET_MANAGER *sheetManager);

/**
 * @brief: Set a set of visual effects for a sheet.
 * @param: (SHEET *)sht: The sheet to set.
 * @param: (uchaf *)buf: The buffer to set for the sheet.
 * @param: (int) xsize: The X size of the buffer.
 * @param: (int) ysize: The Y size of the buffer.
 * @param: (int) colorAndInvisibility: Some flags for the sheet as its name.
 */ 
void setSheetBuffer(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int colorAndInvisibility);

/**
 * @brief: Adjust the height of a sheet.
 * @param: (SHEET *) sheet: The sheet to get set.
 * @param: (int) height: The height.
 */ 
void setSheetHeight(struct SHEET *sht, int height);

/**
 * @brief: Refresh sheets
 * @param: (SHEET_MANAGER *)sheetManager: The sheet manager.
 * @param: (int) bx0: Old X width of box.
 * @param: (int) by0: Old Y height of box.
 * @param: (int) bx1: New X width of box.
 * @param: (int) by1: New Y height of box.
 * @seealso: sheetRefreshSub
 */ 
void sheetRefresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);

/**
 * @brief: Refresh sheets with a ranged position.
 * @param: (SHEET_MANAGER *)sheetManager: The sheet manager.
 * @param: (int) vx0: The left top x position to get refreshed.
 * @param: (int) vy0: The left top y position to get refreshed.
 * @param: (int) vx1: The right bottom x position to get refreshed.
 * @param: (int) vy1: The right bottom y position to get refreshed.
 * @param: (int) leastRefHeight: Only sheets higher than or equal to this height will be updated.
 * @param: (int) maxRefHeight: Only sheets lower than or equal to this height will be updated.
 * @seealso: sheetRefresh
 * @warning: Though asked, it is not very important to keep vx0 < vx1, vy0 < vy1.
 */
void sheetRefreshSub(struct SHEET_MANAGER *sheetManager, int vx0, int vy0, int vx1, int vy1, int leastRefHeight, int maxRefHeight); 

/**
 * @brief: Move a sheet without changing its height.
 * @param: (SHEET *) sht: The sheet to move.
 * @param: (int) vx0: The new x0.
 * @param: (int) vy0: The new y0.
 */ 
void sheetMove(struct SHEET * sht, int vx0, int vy0);

/**
 * @brief: Destroy a sheet (thus it is freed).
 * @param: (SHEET *)sht: The sheet to destroy.
 */ 
void sheetDestroy(struct SHEET *sht);

/**
 * @brief: Refresh the map of sheets.
 * @param: (SHEET_MANAGER *)sheetManager: The sheet manager.
 * @param: (int) vx0: The left top x position to get refreshed.
 * @param: (int) vy0: The left top y position to get refreshed.
 * @param: (int) vx1: The right bottom x position to get refreshed.
 * @param: (int) vy1: The right bottom y position to get refreshed.
 * @param: (int) leastRefHeight: Only sheets higher than or equal to this height will be updated.
 */ 
void refreshSheetMap(struct SHEET_MANAGER *sheetManager, int vx0, int vy0, int vx1, int vy1, int leastRefHeight);

/**
 * @brief: Put a string onto a sheet.
 * @param: (int)x: X position of the string.
 * @param: (int)y: Y position of the string.
 * @param: (int)c: Color of the string.
 * @param: (int)b: Background color.
 * @param: (char *)The string.
 * @param: (int)l: Length of the string.
 */ 
void putStringOnSheet(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);

#endif
