#include "common.h"
#include "m2c.h"

void func_80064464(s32 arg0, s32 arg1, s32 arg2) {
    if (arg1 == (arg2 - 1)) {
        *(M2C_UNK *)((arg1 * 4) + arg0) = 0;
        return;
    }
    *(M2C_UNK *)((arg1 * 4) + arg0) = M2C_FIELD(((arg2 * 4) + arg0), s32 *, -4);
}
