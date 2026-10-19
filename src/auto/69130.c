#include "common.h"
#include "m2c.h"

s16 func_80069D30(s32);                             /* extern */

s16 func_80068530(s32 arg0) {
    return func_80069D30((arg0 + 0x4000) & 0xFFFF);
}
