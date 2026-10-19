#include "common.h"
#include "m2c.h"

typedef struct {
    s32 status;
    u8 pad4[0x5C];
    s32 inode_start_page;
    u8 banks;
} PfsT;

typedef struct {
    u16 ipage[128];
} InodeT;

extern s32 __osCheckId(PfsT *);
extern s32 __osPfsRWInode(PfsT *, InodeT *, u8, u8);

s32 osPfsFreeBlocks(PfsT *pfs, s32 *bytes_not_used)
{
    int j;
    int pages = 0;
    InodeT inode;
    s32 ret = 0;
    u8 bank;
    int offset;

    if (!(pfs->status & 1))
        return 5;
    if ((ret = __osCheckId(pfs)) != 0)
        return ret;
    for (bank = 0; bank < pfs->banks; bank++) {
        if ((ret = __osPfsRWInode(pfs, &inode, 0, bank)) != 0)
            return ret;
        offset = (bank > 0) ? 1 : pfs->inode_start_page;
        for (j = offset; j < 128; j++) {
            if (inode.ipage[j] == 3)
                pages++;
        }
    }
    *bytes_not_used = pages << 8;
    return 0;
}
