#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x60];
    s32 inode_start_page;
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

s32 __osPfsReleasePages(OSPfs *pfs, __OSInode *inode, u8 start_page, u8 bank, __OSInodeUnit *last_page) {
    __OSInodeUnit next_page;
    __OSInodeUnit old_page;
    s32 ret = 0;

    next_page.ipage = (u16)((bank << 8) + start_page);
    do {
        old_page = next_page;
        next_page = inode->inode_page[next_page.inode_t.page];
        inode->inode_page[old_page.inode_t.page].ipage = 3;
    } while (next_page.ipage >= pfs->inode_start_page && next_page.inode_t.bank == bank);
    *last_page = next_page;
    return ret;
}
