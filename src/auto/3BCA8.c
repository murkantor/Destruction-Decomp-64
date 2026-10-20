#include "common.h"
#include "m2c.h"

s32 Track_GetGridCell(s32, s32, s32);                   /* extern */
M2C_UNK List_Push(s32, s32);                    /* extern */
extern void *gGame;

void Grid_AddObjAtPos(s32 arg0, void *arg1) {
    void *temp_s0;

    temp_s0 = M2C_FIELD(M2C_FIELD(M2C_FIELD(gGame, void **, 0x1100), void **, 0x70), void **, 0xC);
    List_Push(M2C_FIELD(temp_s0, s32 *, 0x2C) + (Track_GetGridCell(M2C_FIELD(arg1, s32 *, 0), M2C_FIELD(arg1, s32 *, 4), M2C_FIELD(arg1, s32 *, 8)) * 4), arg0);
}
