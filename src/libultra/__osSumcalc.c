#include "common.h"
#include "m2c.h"

u16 __osSumcalc(u8 *ptr, s32 length) {
    s32 i;
    u32 sum = 0;
    u8 *tmp = ptr;

    for (i = 0; i < length; i++) {
        sum += *tmp++;
    }
    return sum & 0xFFFF;
}
