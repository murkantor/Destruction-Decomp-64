#include "common.h"
#include "m2c.h"

M2C_UNK Track_GetGridCell(s32, s32, s32);               /* extern */

void Car_GetGridCell(void *arg0) {
    Track_GetGridCell(M2C_FIELD(arg0, s32 *, 0x290), M2C_FIELD(arg0, s32 *, 0x294), M2C_FIELD(arg0, s32 *, 0x298));
}

extern void *gGame;

s16 Game_GetMode(void) {
    return M2C_FIELD(gGame, s16 *, 0xC);
}

s32 Mem_Alloc(s32);                                 /* extern */
extern s32 D_800D1C18;
extern s32 D_80124470;

void Surface_InitTable(s32 arg0) {
    D_80124470 = 0;
    D_800D1C18 = Mem_Alloc(arg0 * 4);
}

M2C_UNK Mem_Free(s32);                         /* extern */

void Surface_FreeTable(void) {
    Mem_Free(D_800D1C18);
    D_800D1C18 = 0;
}
