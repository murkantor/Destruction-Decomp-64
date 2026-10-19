#include "common.h"
#include "m2c.h"

void *func_80055A60(s32, M2C_UNK, M2C_UNK);         /* extern */
M2C_UNK func_80055AB4(void *);                      /* extern */
s32 func_80063A20(s32);                             /* extern */
M2C_UNK func_80063B54(s32);                         /* extern */
M2C_UNK func_80063BFC(s32, void *);                 /* extern */
s32 func_80063D6C();                                /* extern */

s32 func_80025238(s32 arg0) {
    s32 temp_a0;
    s32 temp_a0_2;
    s32 temp_a0_3;
    s32 temp_a0_4;
    s32 temp_a0_5;
    s32 temp_a0_6;
    s32 temp_a0_7;
    s32 temp_a0_8;
    s32 temp_a0_9;
    void *temp_v0;

    temp_v0 = func_80055A60(func_80063D6C(), 0x3F, 0);
    func_80063BFC(arg0, temp_v0);
    temp_a0 = M2C_FIELD(temp_v0, s32 *, 0);
    if (temp_a0 != -1) {
        func_80063A20(temp_a0);
    }
    temp_a0_2 = M2C_FIELD(temp_v0, s32 *, 4);
    if (temp_a0_2 != -1) {
        func_80063A20(temp_a0_2);
    }
    temp_a0_3 = M2C_FIELD(temp_v0, s32 *, 8);
    if (temp_a0_3 != -1) {
        func_80063A20(temp_a0_3);
    }
    temp_a0_4 = M2C_FIELD(temp_v0, s32 *, 0xC);
    if (temp_a0_4 != -1) {
        func_80063A20(temp_a0_4);
    }
    temp_a0_5 = M2C_FIELD(temp_v0, s32 *, 0x10);
    if (temp_a0_5 != -1) {
        func_80063A20(temp_a0_5);
    }
    temp_a0_6 = M2C_FIELD(temp_v0, s32 *, 0x14);
    if (temp_a0_6 != -1) {
        func_80063A20(temp_a0_6);
    }
    temp_a0_7 = M2C_FIELD(temp_v0, s32 *, 0x18);
    if (temp_a0_7 != -1) {
        func_80063A20(temp_a0_7);
    }
    temp_a0_8 = M2C_FIELD(temp_v0, s32 *, 0x1C);
    if (temp_a0_8 != -1) {
        func_80063A20(temp_a0_8);
    }
    temp_a0_9 = M2C_FIELD(temp_v0, s32 *, 0x20);
    if (temp_a0_9 != -1) {
        func_80063B54(func_80063A20(temp_a0_9));
    }
    func_80055AB4(temp_v0);
    return 0;
}
