#include "common.h"
#include "m2c.h"

s16 sins(s32);                             /* extern */

s16 coss(s32 arg0) {
    return sins((arg0 + 0x4000) & 0xFFFF);
}
