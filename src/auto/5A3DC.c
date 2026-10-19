#include "common.h"
#include "m2c.h"

s32 osPfsFindFile(void *, u16, s32, s32, s32, s32); /* extern */
extern M2C_UNK D_800B0BB4;
extern u16 D_800BD280;
extern s32 D_800BD284;
extern s32 D_800BD288;
extern s32 D_800BD28C;
extern M2C_UNK D_800BD290;

s32 func_800597DC(s32 arg0, s32 arg1) {
    if (!(*((__typeof__(D_800B0BB4) *)((s8 *)&D_800B0BB4 + (arg0 * 4))) == 0)) {
        return osPfsFindFile((arg0 * 0x68) + (s8 *)&D_800BD290, D_800BD280, D_800BD284, D_800BD288, D_800BD28C, arg1);
    }
    return -1;
}

s32 func_8006E460(void *, u16, s32, s32, s32, s32, s32); /* extern */

s32 func_8005985C(s32 arg0, s32 arg1, s32 arg2) {
    if (!(*((__typeof__(D_800B0BB4) *)((s8 *)&D_800B0BB4 + (arg0 * 4))) == 0)) {
        return func_8006E460((arg0 * 0x68) + (s8 *)&D_800BD290, D_800BD280, D_800BD284, D_800BD288, D_800BD28C, arg1, arg2);
    }
    return -1;
}

s32 func_8006EFB0(void *, u16, s32, s32, s32);      /* extern */

s32 func_800598E0(s32 arg0) {
    if (!(*((__typeof__(D_800B0BB4) *)((s8 *)&D_800B0BB4 + (arg0 * 4))) == 0)) {
        return func_8006EFB0((arg0 * 0x68) + (s8 *)&D_800BD290, D_800BD280, D_800BD284, D_800BD288, D_800BD28C);
    }
    return -1;
}
