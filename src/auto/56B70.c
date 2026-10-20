#include "common.h"
#include "m2c.h"

M2C_UNK osViSetSpecialFeatures(s32);                         /* extern */
M2C_UNK osViSetXScale(f32);                         /* extern */
M2C_UNK osViSetYScale(f32);                         /* extern */
M2C_UNK osViBlack(M2C_UNK);                         /* extern */
M2C_UNK osViSetMode(void *);                        /* extern */
M2C_UNK osWritebackDCache();                 /* extern */
extern void *D_800B0B14;

void func_80055F70(void *arg0) {
    if (arg0 != D_800B0B14) {
        osWritebackDCache(arg0, 0x80);
        if (M2C_FIELD(arg0, u8 *, 0x7D) != 0) {
            osViSetMode(arg0);
            osViBlack(1);
        } else {
            osViBlack(0);
            osViSetMode(arg0);
            osViSetSpecialFeatures(M2C_FIELD(arg0, s32 *, 0x50));
            osViSetXScale(M2C_FIELD(arg0, f32 *, 0x54));
            osViSetYScale(M2C_FIELD(arg0, f32 *, 0x58));
        }
        D_800B0B14 = arg0;
    }
}
