#include "common.h"
#include "m2c.h"

typedef struct {
    s32 status;
    OSMesgQueue *queue;
    s32 channel;
    u8 id[32];
    u8 label[32];
    s32 version;
    s32 dir_size;
    s32 inode_table;
    s32 minode_table;
    s32 dir_table;
    s32 inode_start_page;
    u8 banks;
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

u32 osGetCount(void);
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

s32 __osRepairPackId(OSPfs *pfs, __OSPackId *badid, __OSPackId *newid) {
    s32 ret = 0;
    u8 temp[32];
    u8 comp[32];
    u8 mask = 0;
    s32 i, j = 0;
    u16 index[4];

    newid->repaired = 0xFFFFFFFF;
    newid->random = osGetCount();
    newid->serial_mid = badid->serial_mid;
    newid->serial_low = badid->serial_low;

    if (pfs->activebank != 0) {
        if ((ret = __osPfsSelectBank(pfs, 0)) != 0) {
            return ret;
        }
    }

    do {
        if ((ret = __osPfsSelectBank(pfs, j)) != 0) {
            return ret;
        }
        if ((ret = __osContRamRead(pfs->queue, pfs->channel, 0, temp)) != 0) {
            return ret;
        }
        temp[0] = j | 0x80;
        for (i = 1; i < 32; i++) {
            temp[i] = ~temp[i];
        }
        if ((ret = __osContRamWrite(pfs->queue, pfs->channel, 0, temp, 0)) != 0) {
            return ret;
        }
        if ((ret = __osContRamRead(pfs->queue, pfs->channel, 0, comp)) != 0) {
            return ret;
        }
        for (i = 0; i < 32; i++) {
            if (comp[i] != temp[i]) {
                break;
            }
        }
        if (i != 32) {
            break;
        }
        if (j > 0) {
            if ((ret = __osPfsSelectBank(pfs, 0)) != 0) {
                return ret;
            }
            if ((ret = __osContRamRead(pfs->queue, pfs->channel, 0, temp)) != 0) {
                return ret;
            }
            if (temp[0] != 0x80) {
                break;
            }
        }
        j++;
    } while (j < 62);

    if (pfs->activebank != 0) {
        if ((ret = __osPfsSelectBank(pfs, 0)) != 0) {
            return ret;
        }
    }

    if (j > 0) {
        mask = 1;
    } else {
        mask = 0;
    }
    newid->deviceid = (badid->deviceid & 0xFFFE) | mask;
    newid->banks = j;
    newid->version = badid->version;
    __osIdCheckSum((u16 *)newid, &newid->checksum, &newid->inverted_checksum);

    index[0] = 1;
    index[1] = 3;
    index[2] = 4;
    index[3] = 6;
    for (i = 0; i < 4; i++) {
        if ((ret = __osContRamWrite(pfs->queue, pfs->channel, index[i], (u8 *)newid, 1)) != 0) {
            return ret;
        }
    }
    if ((ret = __osContRamRead(pfs->queue, pfs->channel, 1, temp)) != 0) {
        return ret;
    }
    for (i = 0; i < 32; i++) {
        if (temp[i] != *((u8 *)newid + i)) {
            return 10;
        }
    }
    return 0;
}
