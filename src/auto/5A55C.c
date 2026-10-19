#include "common.h"
#include "m2c.h"

typedef struct {
    u8 data[0x68];
} OSPfs;
extern s32 D_800B0BB4[];
extern OSPfs D_800BD290[];
s32 osPfsFreeBlocks(OSPfs *, s32 *);

s32 func_8005995C(s32 channel, s32 *bytesLeft) {
    if (D_800B0BB4[channel] != 0) {
        return osPfsFreeBlocks(&D_800BD290[channel], bytesLeft);
    }
    return -1;
}
