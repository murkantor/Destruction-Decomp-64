#include "common.h"
#include "m2c.h"

typedef struct {
    s32 status;
    OSMesgQueue *queue;
    s32 channel;
    u8 pad0C[0x65 - 0xC];
    u8 activebank;
} OSPfs;

typedef struct {
    u32 repaired;
    u32 random;
    u64 serial_mid;
    u64 serial_low;
    u16 deviceid;
    u8 banks;
    u8 version;
    u16 checksum;
    u16 inverted_checksum;
} __OSPackId;

s32 __osPfsSelectBank(OSPfs *pfs, u8 bank);
s32 __osContRamRead(OSMesgQueue *mq, s32 channel, u16 address, u8 *buffer);
s32 __osContRamWrite(OSMesgQueue *mq, s32 channel, u16 address, u8 *buffer, s32 force);

static inline void __osIdCheckSum(u16 *ptr, u16 *csum, u16 *icsum) {
    u16 data = 0;
    u32 j;

    *csum = *icsum = 0;
    for (j = 0; j < 28; j += 2) {
        data = *((u16 *)((u32)ptr + j));
        *csum += data;
        *icsum += ~data;
    }
}

s32 __osCheckPackId(OSPfs *pfs, __OSPackId *temp) {
    u16 index[4];
    s32 ret = 0;
    u16 sum;
    u16 isum;
    s32 i;
    s32 j;

    if (pfs->activebank != 0) {
        if ((ret = __osPfsSelectBank(pfs, 0)) != 0) {
            return ret;
        }
    }
    index[0] = 1;
    index[1] = 3;
    index[2] = 4;
    index[3] = 6;
    for (i = 1; i < 4; i++) {
        if ((ret = __osContRamRead(pfs->queue, pfs->channel, index[i], (u8 *)temp)) != 0) {
            return ret;
        }
        __osIdCheckSum((u16 *)temp, &sum, &isum);
        if (temp->checksum != sum || temp->inverted_checksum != isum) {
            continue;
        }
        break;
    }
    if (i == 4) {
        return 10;
    }
    for (j = 0; j < 4; j++) {
        if (j != i) {
            if ((ret = __osContRamWrite(pfs->queue, pfs->channel, index[j], (u8 *)temp, 1)) != 0) {
                return ret;
            }
        }
    }
    return 0;
}
