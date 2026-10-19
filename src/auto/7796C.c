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

typedef union {
    struct {
        u8 bank;
        u8 page;
    } inode_t;
    u16 ipage;
} __OSInodeUnit;

typedef struct {
    __OSInodeUnit inode_page[128];
} __OSInode;

extern __OSInode D_80130F00;
extern s32 D_800B6D90;
extern u8 __osPfsInodeCacheBank;
extern void bcopy(const void *, void *, s32);
extern s32 __osPfsSelectBank(OSPfs *, u8);
extern s32 __osContRamWrite(OSMesgQueue *, s32, u16, u8 *, s32);
extern s32 __osContRamRead(OSMesgQueue *, s32, u16, u8 *);

static inline u16 sumcalc(u8 *ptr, int length) {
    int i;
    u32 sum = 0;
    u8 *tmp = ptr;
    for (i = 0; i < length; i++) {
        sum += *tmp++;
    }
    return sum;
}

s32 __osPfsRWInode(OSPfs *pfs, __OSInode *inode, u8 flag, u8 bank) {
    u8 sum;
    int j;
    s32 ret;
    int offset;
    u8 *addr;

    if (flag == 0 && bank == __osPfsInodeCacheBank && pfs->channel == D_800B6D90) {
        bcopy(&D_80130F00, inode, sizeof(__OSInode));
        return 0;
    }

    if (pfs->activebank != 0) {
        ret = __osPfsSelectBank(pfs, 0);
        if (ret != 0) {
            return ret;
        }
    }

    offset = (bank > 0) ? 1 : pfs->inode_start_page;

    if (flag == 1) {
        inode->inode_page[0].inode_t.page = sumcalc((u8 *)(inode->inode_page + offset), (128 - offset) * 2);
    }

    for (j = 0; j < 8; j++) {
        addr = (u8 *)inode->inode_page + j * 32;
        if (flag == 1) {
            ret = __osContRamWrite(pfs->queue, pfs->channel, pfs->inode_table + bank * 8 + j, addr, 0);
            ret = __osContRamWrite(pfs->queue, pfs->channel, pfs->minode_table + bank * 8 + j, addr, 0);
        } else {
            ret = __osContRamRead(pfs->queue, pfs->channel, pfs->inode_table + bank * 8 + j, addr);
        }
        if (ret != 0) {
            return ret;
        }
    }

    if (flag == 0) {
        sum = sumcalc((u8 *)(inode->inode_page + offset), (128 - offset) * 2);
        if (sum != inode->inode_page[0].inode_t.page) {
            for (j = 0; j < 8; j++) {
                addr = (u8 *)inode->inode_page + j * 32;
                ret = __osContRamRead(pfs->queue, pfs->channel, pfs->minode_table + bank * 8 + j, addr);
            }
            sum = sumcalc((u8 *)(inode->inode_page + offset), (128 - offset) * 2);
            if (sum != inode->inode_page[0].inode_t.page) {
                return 3;
            }
            for (j = 0; j < 8; j++) {
                addr = (u8 *)inode->inode_page + j * 32;
                ret = __osContRamWrite(pfs->queue, pfs->channel, pfs->inode_table + bank * 8 + j, addr, 0);
            }
        }
    }

    __osPfsInodeCacheBank = bank;
    bcopy(inode, &D_80130F00, sizeof(__OSInode));
    D_800B6D90 = pfs->channel;
    return 0;
}
