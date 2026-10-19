#include "common.h"
#include "m2c.h"

M2C_UNK func_80058B78();                            /* extern */
M2C_UNK func_8006CF60(M2C_UNK *, M2C_UNK *);        /* extern */
extern M2C_UNK D_800BBAF8;
extern M2C_UNK D_800BBB34;
extern M2C_UNK D_800D1090;
extern s32 D_80125C8C;

void func_80053EA8(void) {
    func_8006CF60(&D_800D1090, &D_800BBAF8);
    do {
        func_80058B78();
    } while (!(D_80125C8C & 0x1000));
    func_8006CF60(&D_800D1090, &D_800BBB34);
}

void DebugMenu_Init(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0) = 0;
    M2C_FIELD(arg0, s32 *, 4) = 0;
}

void DebugMenu_AddToggle(void *arg0, s32 arg1, s32 arg2, s8 arg3, u8 arg4, s32 arg5) {
    s32 temp_v1;
    void *temp_v0;

    temp_v1 = M2C_FIELD(arg0, s32 *, 4);
    if (temp_v1 < 0x14) {
        M2C_FIELD(arg0, s32 *, 4) = (s32) (temp_v1 + 1);
        temp_v0 = arg0 + (temp_v1 * 0x18);
        M2C_FIELD(temp_v0, s32 *, 8) = 1;
        M2C_FIELD(temp_v0, s32 *, 0xC) = arg1;
        M2C_FIELD(temp_v0, s32 *, 0x10) = arg2;
        M2C_FIELD(temp_v0, s8 *, 0x14) = arg3;
        M2C_FIELD(temp_v0, u8 *, 0x15) = arg4;
        M2C_FIELD(temp_v0, s32 *, 0x18) = arg5;
    }
}

void DebugMenu_AddInt(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    s32 temp_v1;
    void *temp_v0;

    temp_v1 = M2C_FIELD(arg0, s32 *, 4);
    if (temp_v1 < 0x14) {
        M2C_FIELD(arg0, s32 *, 4) = (s32) (temp_v1 + 1);
        temp_v0 = arg0 + (temp_v1 * 0x18);
        M2C_FIELD(temp_v0, s32 *, 8) = 0;
        M2C_FIELD(temp_v0, s32 *, 0xC) = arg1;
        M2C_FIELD(temp_v0, s32 *, 0x10) = arg2;
        M2C_FIELD(temp_v0, s32 *, 0x14) = arg3;
        M2C_FIELD(temp_v0, s32 *, 0x18) = arg4;
        M2C_FIELD(temp_v0, s32 *, 0x1C) = arg5;
    }
}

void DebugMenu_AddFloat(void *arg0, s32 arg1, s32 arg2, f32 arg3) {
    s32 temp_v1;
    void *temp_v0;

    temp_v1 = M2C_FIELD(arg0, s32 *, 4);
    if (temp_v1 < 0x14) {
        M2C_FIELD(arg0, s32 *, 4) = (s32) (temp_v1 + 1);
        temp_v0 = arg0 + (temp_v1 * 0x18);
        M2C_FIELD(temp_v0, s32 *, 8) = 2;
        M2C_FIELD(temp_v0, s32 *, 0xC) = arg1;
        M2C_FIELD(temp_v0, s32 *, 0x10) = arg2;
        M2C_FIELD(temp_v0, f32 *, 0x1C) = arg3;
    }
}

void DebugMenu_AddSubmenu(void *arg0, s32 arg1, s32 arg2) {
    s32 temp_v1;
    void *temp_v0;

    temp_v1 = M2C_FIELD(arg0, s32 *, 4);
    if (temp_v1 < 0x14) {
        M2C_FIELD(arg0, s32 *, 4) = (s32) (temp_v1 + 1);
        temp_v0 = arg0 + (temp_v1 * 0x18);
        M2C_FIELD(temp_v0, s32 *, 8) = 3;
        M2C_FIELD(temp_v0, s32 *, 0xC) = arg1;
        M2C_FIELD(temp_v0, s32 *, 0x10) = arg2;
    }
}
