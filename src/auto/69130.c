#include "common.h"
#include "m2c.h"

s16 sins(s32);                             /* extern */

s16 func_80068530(s32 arg0) {
    return sins((arg0 + 0x4000) & 0xFFFF);
}
