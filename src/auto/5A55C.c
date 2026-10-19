#include "common.h"
#include "m2c.h"

typedef struct {
    u8 data[0x68];
} OSPfs;
extern s32 gPakReady[];
extern OSPfs gPakFs[];
s32 osPfsFreeBlocks(OSPfs *, s32 *);

s32 Pak_FreeBlocks(s32 channel, s32 *bytesLeft) {
    if (gPakReady[channel] != 0) {
        return osPfsFreeBlocks(&gPakFs[channel], bytesLeft);
    }
    return -1;
}
