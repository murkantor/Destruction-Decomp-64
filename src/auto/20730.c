#include "common.h"
#include "m2c.h"

s32 func_8001FB30(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 0xA54);
}

s32 func_8001FB38(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 0xA58);
}

M2C_UNK func_8005A448(f32 *, s32);                  /* extern */

f32 func_8001FB40(s32 arg0) {
    f32 sp10;

    func_8005A448(&sp10, arg0 + 0x224);
    return sp10;
}

M2C_UNK func_8005A448(f32 *, s32);                  /* extern */

f32 func_8001FB64(s32 arg0) {
    f32 sp10;

    func_8005A448(&sp10, arg0 + 0x224);
    return sp10;
}
