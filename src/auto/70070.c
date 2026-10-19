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
s32 func_80076698(OSPfs *pfs, __OSPackId *badid, __OSPackId *newid);
void bcopy(const void *src, void *dst, s32 len);
void __osSiGetAccess(void);
void __osSiRelAccess(void);
s32 __osPfsGetStatus(OSMesgQueue *queue, s32 channel);
s32 __osIdCheckSum(u16 *ptr, u16 *csum, u16 *icsum);
s32 osPfsChecker(OSPfs *pfs);

s32 osPfsInitPak(OSMesgQueue *queue, OSPfs *pfs, s32 channel) {
    s32 ret = 0;
    u16 sum, isum;
    u8 temp[32];
    __OSPackId *id;
    __OSPackId newid;

    __osSiGetAccess();
    ret = __osPfsGetStatus(queue, channel);
    __osSiRelAccess();
    if (ret != 0) return ret;
    pfs->queue = queue;
    pfs->channel = channel;
    pfs->status = 0;
    ret = __osPfsSelectBank(pfs, 0);
    if (ret != 0) return ret;
    ret = __osContRamRead(pfs->queue, pfs->channel, 1, temp);
    if (ret != 0) return ret;
    __osIdCheckSum((u16 *)temp, &sum, &isum);
    id = (__OSPackId *)temp;
    if (id->checksum != sum || id->inverted_checksum != isum) {
        if ((ret = __osCheckPackId(pfs, id)) != 0) {
            pfs->status |= 4;
            return ret;
        }
    }
    if ((id->deviceid & 1) == 0) {
        ret = func_80076698(pfs, id, &newid);
        if (ret) {
            if (ret == 10)
                pfs->status |= 4;
            return ret;
        }
        id = &newid;
        if ((id->deviceid & 1) == 0)
            return 11;
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
    ret = osPfsChecker(pfs);
    pfs->status |= 1;
    return ret;
}
