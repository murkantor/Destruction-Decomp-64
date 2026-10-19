#include "common.h"
#include "m2c.h"

typedef struct {
    u8 data[0x68];
} OSPfs;
extern s32 gPakReady[];
extern OSPfs gPakFs[];
s32 osPfsReadWriteFile(OSPfs *, s32, u8, s32, s32, u8 *);

s32 Pak_ReadFile(s32 channel, s32 fileNo, s32 offset, s32 size, u8 *buf) {
    if (gPakReady[channel] != 0) {
        return osPfsReadWriteFile(&gPakFs[channel], fileNo, 0, offset, size, buf);
    }
    return -1;
}
