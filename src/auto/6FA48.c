#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x60];
    s32 inode_start_page;
    u8 banks;
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

typedef struct {
    __OSInode inode;
    u8 bank;
    u8 map[256];
} __OSInodeCache;

s32 __osPfsRWInode(OSPfs *pfs, __OSInode *inode, u8 flag, u8 bank);

s32 corrupted(OSPfs *pfs, __OSInodeUnit fpage, __OSInodeCache *cache) {
    s32 j;
    s32 hit = 0;
    u8 bank;
    s32 offset;
    s32 ret = 0;
    s32 idx = (fpage.inode_t.page / 4) + ((fpage.inode_t.bank % 8) * 32);

    for (bank = 0; bank < pfs->banks; bank++) {
        offset = (bank > 0) ? 1 : pfs->inode_start_page;
        if (bank == fpage.inode_t.bank ||
            (cache->map[idx] & (1 << (bank % 8)))) {
            if (bank != cache->bank) {
                ret = __osPfsRWInode(pfs, &cache->inode, 0, bank);
                if (ret != 0 && ret != 3) {
                    return ret;
                }
                cache->bank = bank;
            }
            for (j = offset; hit < 2 && j < 128; j++) {
                if (cache->inode.inode_page[j].ipage == fpage.ipage) {
                    hit++;
                }
            }
            if (1 < hit) {
                return 2;
            }
        }
    }
    return hit;
}
