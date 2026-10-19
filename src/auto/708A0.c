#include "common.h"
#include "m2c.h"

typedef struct {
    s32 status;
    void *queue;
    s32 channel;
    u8 pad[0x65 - 0xC];
    u8 activebank;
} OSPfs;
s32 __osContRamWrite(void *, s32, u16, u8 *, s32);

s32 __osPfsSelectBank(OSPfs *pfs, u8 bank) {
    u8 temp[32];
    int i;
    s32 ret = 0;

    for (i = 0; i < 32; i++) {
        temp[i] = bank;
    }
    ret = __osContRamWrite(pfs->queue, pfs->channel, 0x400, temp, 0);
    if (ret == 0) {
        pfs->activebank = bank;
    }
    return ret;
}
