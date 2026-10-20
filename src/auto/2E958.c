#include "common.h"
#include "m2c.h"

M2C_UNK Mtx_FromRotPosScaled(void *, s32, s32, s32, void *); /* extern */
s32 Model_Draw(void *, s32);                     /* extern */
extern s32 D_80095890;

s32 Prop_Draw(void *arg0, s32 arg1) {
    void *temp_s0;

    if ((M2C_FIELD(arg0, s32 *, 0x60) != 0) && (D_80095890 != 0)) {
        temp_s0 = arg0 + 0x78;
        Mtx_FromRotPosScaled(temp_s0, M2C_FIELD(arg0, s32 *, 4), M2C_FIELD(arg0, s32 *, 8), M2C_FIELD(arg0, s32 *, 0xC), arg0 + 0x10);
        return Model_Draw(temp_s0, arg1);
    }
    return arg1;
}
