#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x60];
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

s32 corrupted_init(OSPfs *pfs, __OSInodeCache *cache) {
    s32 i;
    s32 n;
    s32 offset;
    u8 bank;
    __OSInodeUnit tpage;
    __OSInode tmp_inode;
    s32 ret;

    for (i = 0; i < 256; i++) {
        cache->map[i] = 0;
    }
    cache->bank = -1;
    for (bank = 0; bank < pfs->banks; bank++) {
        offset = (bank > 0) ? 1 : pfs->inode_start_page;
        ret = __osPfsRWInode(pfs, &tmp_inode, 0, bank);
        if (ret != 0 && ret != 3) {
            return ret;
        }
        for (i = offset; i < 128; i++) {
            tpage = tmp_inode.inode_page[i];
            if (tpage.ipage >= pfs->inode_start_page && tpage.inode_t.bank != bank) {
                n = tpage.inode_t.page / 4 + (tpage.inode_t.bank % 8) * 32;
                cache->map[n] |= (1 << (bank % 8));
            }
        }
    }
    return 0;
}
