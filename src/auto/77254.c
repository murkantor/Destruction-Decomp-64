#include "common.h"
#include "m2c.h"

s32 __osIdCheckSum(u16 *ptr, u16 *csum, u16 *icsum) {
    u16 *end = (u16 *)((u8 *)ptr + 0x1C);
    u16 data;

    *icsum = 0;
    *csum = 0;
    do {
        data = *ptr;
        *csum += data;
        ptr++;
        *icsum += ~data;
    } while (ptr < end);
    return 0;
}
