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

s32 __osPfsSelectBank(OSPfs *pfs, u8 bank);
s32 __osContRamRead(OSMesgQueue *mq, s32 channel, u16 address, u8 *buffer);
s32 __osCheckPackId(OSPfs *pfs, __OSPackId *id);
s32 __osRepairPackId(OSPfs *pfs, __OSPackId *badid, __OSPackId *newid);
void bcopy(const void *src, void *dst, s32 len);

static inline s32 idsum(u16 *ptr, u16 *csum, u16 *icsum) {
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

s32 __osGetId(OSPfs *pfs) {
    u16 sum, isum;
    u8 temp[32];
    __OSPackId newid;
    s32 ret;
    __OSPackId *id;

    if (pfs->activebank != 0) {
        ret = __osPfsSelectBank(pfs, 0);
        if (ret != 0) return ret;
    }
    ret = __osContRamRead(pfs->queue, pfs->channel, 1, temp);
    if (ret != 0) return ret;
    idsum((u16 *)temp, &sum, &isum);
    id = (__OSPackId *)temp;
    if (id->checksum != sum || id->inverted_checksum != isum) {
        ret = __osCheckPackId(pfs, id);
        if (ret == 10) {
            ret = __osRepairPackId(pfs, id, &newid);
            if (ret != 0) return ret;
            id = &newid;
        } else if (ret != 0) {
            return ret;
        }
    }
    if ((id->deviceid & 1) == 0) {
        ret = __osRepairPackId(pfs, id, &newid);
        if (ret != 0) return ret;
        id = &newid;
        if ((id->deviceid & 1) == 0) return 11;
    }
    bcopy(id, pfs->id, 32);
    pfs->version = id->version;
    pfs->banks = id->banks;
    pfs->inode_start_page = 1 + 2 + (2 * pfs->banks);
    pfs->dir_size = 2 * 8;
    pfs->inode_table = 1 * 8;
    pfs->minode_table = (1 + pfs->banks) * 8;
    pfs->dir_table = pfs->minode_table + (pfs->banks * 8);
    ret = __osContRamRead(pfs->queue, pfs->channel, 7, pfs->label);
    if (ret != 0) return ret;
    return 0;
}
