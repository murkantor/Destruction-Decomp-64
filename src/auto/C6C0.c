#include "common.h"
#include "m2c.h"

void func_8000BAC0(void) {

}

s32 func_8000BAC8(s32 arg0) {
    return arg0;
}

M2C_UNK func_8005AB1C(s32, s32, s32, s32);          /* extern */
M2C_UNK func_8005AB4C(s32, M2C_UNK);                /* extern */
M2C_UNK Model_Draw(s32, s32);                    /* extern */
extern s32 D_80123324;

void func_8000BAD0(s32 arg0, void *arg1) {
    func_8005AB1C(D_80123324, M2C_FIELD(arg1, s32 *, 0), M2C_FIELD(arg1, s32 *, 4), M2C_FIELD(arg1, s32 *, 8));
    func_8005AB4C(D_80123324, 0x3CA3D70A);
    Model_Draw(D_80123324, arg0);
}
