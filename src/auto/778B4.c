#include "common.h"
#include "m2c.h"

typedef struct {
    s32 status;
    OSMesgQueue *queue;
    s32 channel;
    u8 id[32];
    u8 pad2C[0x65 - 0x2C];
    u8 activebank;
} OSPfs;
extern s32 __osPfsSelectBank(OSPfs *pfs, u8 bank);
extern s32 __osContRamRead(OSMesgQueue *mq, s32 channel, u16 address, u8 *buffer);
extern s32 bcmp(void *a, void *b, u32 n);

s32 __osCheckId(OSPfs *pfs) {
    u8 temp[32];
    s32 ret;

    if (pfs->activebank != 0) {
        ret = __osPfsSelectBank(pfs, 0);
        if (ret == 2) {
            ret = __osPfsSelectBank(pfs, 0);
        }
        if (ret != 0) {
            return ret;
        }
    }
    ret = __osContRamRead(pfs->queue, pfs->channel, 1, temp);
    if (ret != 0) {
        if (ret != 2) {
            return ret;
        }
        ret = __osContRamRead(pfs->queue, pfs->channel, 1, temp);
        if (ret != 0) {
            return ret;
        }
    }
    if (bcmp(pfs->id, temp, 32) != 0) {
        return 2;
    }
    return 0;
}
