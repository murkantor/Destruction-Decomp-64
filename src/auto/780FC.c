#include "common.h"
#include "m2c.h"

u8 __osContDataCrc(u8 *data) {
    s32 ret;
    u32 bit;
    u32 byte;

    ret = 0;
    for (byte = 32; byte; byte--, data++) {
        for (bit = 0x80; bit; bit >>= 1) {
            ret <<= 1;
            if (*data & bit) {
                if (ret & 0x100) {
                    ret ^= 0x84;
                } else {
                    ret++;
                }
            } else {
                if (ret & 0x100) {
                    ret ^= 0x85;
                }
            }
        }
    }
    do {
        ret <<= 1;
        if (ret & 0x100) {
            ret ^= 0x85;
        }
        byte++;
    } while (byte < 8U);
    return ret;
}
