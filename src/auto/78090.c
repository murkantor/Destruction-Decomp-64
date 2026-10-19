#include "common.h"
#include "m2c.h"

u8 __osContAddressCrc(u16 addr) {
    u32 ret = 0;
    u32 bit;
    u32 i;

    for (bit = 0x400; bit; bit /= 2) {
        ret *= 2;
        if (addr & bit) {
            if (ret & 0x20) {
                ret ^= 0x14;
            } else {
                ++ret;
            }
        } else {
            if (ret & 0x20) {
                ret ^= 0x15;
            }
        }
    }
    for (bit = 5; bit != 0; bit--) {
        ret <<= 1;
        if (ret & 0x20) {
            ret ^= 0x15;
        }
    }
    return ret & 0x1F;
}
