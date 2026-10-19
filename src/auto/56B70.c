#include "common.h"
#include "m2c.h"

M2C_UNK func_8006C1F0(s32);                         /* extern */
M2C_UNK func_8006C360(f32);                         /* extern */
M2C_UNK func_8006C430(f32);                         /* extern */
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
            func_8006C1F0(M2C_FIELD(arg0, s32 *, 0x50));
            func_8006C360(M2C_FIELD(arg0, f32 *, 0x54));
            func_8006C430(M2C_FIELD(arg0, f32 *, 0x58));
        }
        D_800B0B14 = arg0;
    }
}
