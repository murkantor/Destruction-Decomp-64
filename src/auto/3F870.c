#include "common.h"
#include "m2c.h"

s32 Font_GetStringWidth();                                /* extern */
extern f64 D_800BA370;

f32 Font_CenterX(M2C_UNK arg0, M2C_UNK arg1, f32 arg2) {
    return (f32) ((f64) arg2 - ((f64) (f32) Font_GetStringWidth(arg0, arg1) * D_800BA370));
}

s32 Font_GetStringWidth();                                /* extern */

f32 Font_RightAlignX(M2C_UNK arg0, M2C_UNK arg1, f32 arg2) {
    return arg2 - (f32) Font_GetStringWidth(arg0, arg1);
}
