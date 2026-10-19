#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xC]; s16 xC; } Game;
typedef struct { s32 used; u8 pad4[0x1C]; } SaveSlot;
extern Game *gGame;
extern SaveSlot gSaveSlots[16];
extern s32 D_80097D80;
extern s32 D_80125CC4;
void func_80063380(s32);
void func_80063564(s32, s32);
void Pak_RefreshSaveSlots(void);

s32 Menu_HandleSelect(u32 arg0) {
    s32 ret = 0;
    s32 count = 0;
    s32 i;

    switch (arg0) {
        case 0:
            ret = 1;
            break;
        case 2:
            if (gGame->xC != 1) {
                func_80063380(1);
            }
            func_80063564(0xE, 1);
            break;
        case 5:
            Pak_RefreshSaveSlots();
            for (i = 0; i < 16; i++) {
                count += gSaveSlots[i].used != 0;
            }
            if (count == 16 || D_80097D80 / 256 < 4) {
                if (gGame->xC != 1) {
                    func_80063380(1);
                }
                func_80063564(0xC, 1);
            } else {
                if (gGame->xC != 1) {
                    func_80063380(1);
                }
                func_80063564(7, 1);
            }
            break;
        case 7:
            if (gGame->xC != 1) {
                func_80063380(1);
            }
            func_80063564(0xC, 1);
            break;
        case 8:
            if (gGame->xC != 1) {
                func_80063380(1);
            }
            func_80063564(0xC, 1);
            break;
        case 10:
            if (D_80125CC4 == 0) {
                ret = 0;
                break;
            }
        case 3:
        case 4:
            if (gGame->xC != 1) {
                func_80063380(1);
            }
            func_80063564(0xD, 1);
            break;
    }
    return ret;
}
