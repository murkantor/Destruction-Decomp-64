#include "common.h"
#include "m2c.h"

typedef union {
    struct { u8 bank; u8 page; } inode_t;
    u16 ipage;
} __OSInodeUnit;
typedef struct { __OSInodeUnit inode_page[128]; } __OSInode;
typedef struct { u8 pad[0x60]; s32 inode_start_page; } OSPfs;

s32 func_8006E76C(OSPfs *pfs, __OSInode *inode, int file_size_in_pages, int *first_page, u8 bank, int *decleared, int *last_page) {
    int j;
    int spage, old_page;
    int ret = 0;
    int offset = bank > 0 ? 1 : pfs->inode_start_page;

    for (j = offset; j < 128; j++) {
        if (inode->inode_page[j].ipage == 3)
            break;
    }

    if (j == 128) {
        *first_page = -1;
        return ret;
    }

    spage = j;
    *decleared = 1;
    old_page = j;
    j++;

    while (file_size_in_pages > *decleared && j < 128) {
        if (inode->inode_page[j].ipage == 3) {
            inode->inode_page[old_page].inode_t.bank = bank;
            inode->inode_page[old_page].inode_t.page = j;
            old_page = j;
            *decleared += 1;
        }
        j++;
    }

    *first_page = spage;

    if (j == 128 && file_size_in_pages > *decleared)
        *last_page = old_page;
    else {
        inode->inode_page[old_page].ipage = 1;
        *last_page = 0;
    }
    return ret;
}
