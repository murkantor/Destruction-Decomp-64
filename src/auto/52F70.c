#include "common.h"
#include "m2c.h"

M2C_UNK memcpy(s32, s32, s32);               /* extern */
extern s32 D_800B048C;
extern s32 D_800BCA4C;
extern s32 D_800BCA50;
extern u32 D_800BCA54;

void Inflate_FlushWindow(s32 arg0, s32 arg1) {
    if ((u32) D_800BCA54 >= (u32) (D_800BCA50 + arg1)) {
        memcpy(D_800BCA4C + D_800BCA50, arg0, arg1);
        PORT_MARK_BE(D_800BCA4C + D_800BCA50, arg1);
        D_800BCA50 += arg1;
        return;
    }
    D_800B048C = 1;
}
