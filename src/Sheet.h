/** Window Sheets Management
 * 
 * @author: Leon Wong
 * @build: 2018083010453
 * @brief: This files contains some functions about window management.
 * @usage: This files shoule be both included and compiled.
 * 
 */ 

#if (!defined(SHEET_H))
#define SHEET_H 1

#include "Memorie.h"
#include "include/stdio.h"

/// The max number of sheets.
#define MAX_SHEETS 256

/// The FLAGS for sheets:
const static int SHEET_FREE = 0;
const static int SHEET_IN_USE = 1;

struct SHEET;
struct SHEET_MANAGER;

typedef struct SHEET SHEET;
typedef struct SHEET_MANAGER SHEET_MANAGER;

/// A struct about one sheet.
struct SHEET {
    unsigned char *buf; // Buffer of this sheet.
    int bxsize; // X size.
    int bysize; // Y size.
    int vx0; // Position (Left top)
    int vy0; // Position (Left top)
    int colorAndInvisibility; // Just as its name.
    int height; // The order of the sheet from top to bottom.
    int flags;
    SHEET_MANAGER *ctl;
};

/// Sheet manager
struct SHEET_MANAGER{
    unsigned char *vram; // Address of VRAM
    unsigned char *map; // A map of the screen indicating which pixels' height.
    int xsize; // The X size of the UI
    int ysize; // The Y size of the UI
    int top; // The max number of layers currently. (i.e. height of the highest sheet)
    SHEET *sheets[MAX_SHEETS]; // Table of sheets after ordering.
    SHEET sheets0[MAX_SHEETS]; // Array to contain all sheets.
};

/**
 * @brief: Init a SheetManager and return it.
 * @param: (MEMORY*) memman: Memory Manager.
 * @param: (uchar *) vram: Address of VRAM.
 * @param: (int) xsize: X resolution.
 * @param: (int) ysize: Y resolution.
 */ 
SHEET_MANAGER* initSheetManager(MEMORY_FREE_TABLE *memman, unsigned char *vram, int xsize, int ysize);

/**
 * @brief: Alloc a sheet for a window.
 * @param: (SHEET_MANAGER *) sheetManager: The sheet manager pointer.
 * @return (SHEET *) A sheet assigned for the window.
 * @warning: This may return a NULL when there's no available sheets!
 */
SHEET *allocASheetForWindow(SHEET_MANAGER *sheetManager);

/**
 * @brief: Set a set of visual effects for a sheet.
 * @param: (SHEET *)sht: The sheet to set.
 * @param: (uchaf *)buf: The buffer to set for the sheet.
 * @param: (int) xsize: The X size of the buffer.
 * @param: (int) ysize: The Y size of the buffer.
 * @param: (int) colorAndInvisibility: Some flags for the sheet as its name.
 */ 
void setSheetBuffer(SHEET *sht, unsigned char *buf, int xsize, int ysize, int colorAndInvisibility);

/**
 * @brief: Adjust the height of a sheet.
 * @param: (SHEET *) sheet: The sheet to get set.
 * @param: (int) height: The height.
 */ 
void setSheetHeight(SHEET *sht, int height);

/**
 * @brief: Refresh sheets
 * @param: (SHEET_MANAGER *)sheetManager: The sheet manager.
 * @param: (int) bx0: Old X width of box.
 * @param: (int) by0: Old Y height of box.
 * @param: (int) bx1: New X width of box.
 * @param: (int) by1: New Y height of box.
 * @seealso: sheetRefreshSub
 */ 
void sheetRefresh(SHEET *sht, int bx0, int by0, int bx1, int by1);

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
void sheetRefreshSub(SHEET_MANAGER *sheetManager, int vx0, int vy0, int vx1, int vy1, int leastRefHeight, int maxRefHeight); 

/**
 * @brief: Move a sheet without changing its height.
 * @param: (SHEET *) sht: The sheet to move.
 * @param: (int) vx0: The new x0.
 * @param: (int) vy0: The new y0.
 */ 
void sheetMove(SHEET * sht, int vx0, int vy0);

/**
 * @brief: Destroy a sheet (thus it is freed).
 * @param: (SHEET *)sht: The sheet to destroy.
 */ 
void sheetDestroy(SHEET *sht);

/**
 * @brief: Refresh the map of sheets.
 * @param: (SHEET_MANAGER *)sheetManager: The sheet manager.
 * @param: (int) vx0: The left top x position to get refreshed.
 * @param: (int) vy0: The left top y position to get refreshed.
 * @param: (int) vx1: The right bottom x position to get refreshed.
 * @param: (int) vy1: The right bottom y position to get refreshed.
 * @param: (int) leastRefHeight: Only sheets higher than or equal to this height will be updated.
 */ 
void refreshSheetMap(SHEET_MANAGER *sheetManager, int vx0, int vy0, int vx1, int vy1, int leastRefHeight);

#endif
