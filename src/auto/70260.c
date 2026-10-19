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

typedef struct {
    u32 game_code;
    u16 company_code;
    __OSInodeUnit start_page;
    u8 status;
    s8 reserved;
    u16 data_sum;
    u8 ext_name[4];
    u8 game_name[16];
} __OSDir;

extern s32 __osCheckId(OSPfs *);
extern s32 __osPfsRWInode(OSPfs *, __OSInode *, u8, u8);
extern s32 __osPfsSelectBank(OSPfs *, u8);
extern s32 __osContRamWrite(OSMesgQueue *, s32, u16, u8 *, s32);
extern s32 __osContRamRead(OSMesgQueue *, s32, u16, u8 *);

#define CHECK_IPAGE(p) (((p).ipage >= pfs->inode_start_page) && ((p).inode_t.bank < pfs->banks) && ((p).inode_t.page >= 0x01) && ((p).inode_t.page < 0x80))

static inline s32 __osPfsGetNextPage(OSPfs *pfs, u8 *bank, __OSInode *inode, __OSInodeUnit *page) {
    s32 ret;
    if (page->inode_t.bank != *bank) {
        *bank = page->inode_t.bank;
        ret = __osPfsRWInode(pfs, inode, 0, *bank);
        if (ret != 0) {
            return ret;
        }
    }
    *page = inode->inode_page[page->inode_t.page];
    if (!CHECK_IPAGE(*page)) {
        if (page->ipage == 1) {
            return 5;
        }
        return 3;
    }
    return 0;
}

s32 osPfsReadWriteFile(OSPfs *pfs, s32 file_no, u8 flag, int offset, int size_in_bytes, u8 *data_buffer) {
    s32 ret;
    __OSDir dir;
    __OSInode inode;
    __OSInodeUnit cur_page;
    int cur_block;
    int siz_block;
    u8 *buffer;
    u8 bank;
    u16 blockno;

    if (file_no >= pfs->dir_size || file_no < 0) {
        return 5;
    }
    if (size_in_bytes <= 0 || (size_in_bytes % 32) != 0) {
        return 5;
    }
    if (offset < 0 || (offset % 32) != 0) {
        return 5;
    }
    if (!(pfs->status & 1)) {
        return 5;
    }
    if (__osCheckId(pfs) == 2) {
        return 2;
    }
    if (pfs->activebank != 0) {
        ret = __osPfsSelectBank(pfs, 0);
        if (ret != 0) {
            return ret;
        }
    }
    ret = __osContRamRead(pfs->queue, pfs->channel, pfs->dir_table + file_no, (u8 *)&dir);
    if (ret != 0) {
        return ret;
    }
    if (dir.company_code == 0 || dir.game_code == 0) {
        return 5;
    }
    if (!CHECK_IPAGE(dir.start_page)) {
        if (dir.start_page.ipage == 1) {
            return 5;
        }
        return 3;
    }
    if (flag == 0 && (dir.status & 2) == 0) {
        return 6;
    }
    bank = 255;
    cur_block = offset / 32;
    cur_page = dir.start_page;
    while (cur_block >= 8) {
        ret = __osPfsGetNextPage(pfs, &bank, &inode, &cur_page);
        if (ret != 0) {
            return ret;
        }
        cur_block -= 8;
    }
    siz_block = size_in_bytes / 32;
    buffer = data_buffer;
    while (siz_block > 0) {
        if (cur_block == 8) {
            ret = __osPfsGetNextPage(pfs, &bank, &inode, &cur_page);
            if (ret != 0) {
                return ret;
            }
            cur_block = 0;
        }
        if (pfs->activebank != cur_page.inode_t.bank) {
            ret = __osPfsSelectBank(pfs, cur_page.inode_t.bank);
            if (ret != 0) {
                return ret;
            }
        }
        blockno = cur_page.inode_t.page * 8 + cur_block;
        if (flag == 0) {
            ret = __osContRamRead(pfs->queue, pfs->channel, blockno, buffer);
        } else {
            ret = __osContRamWrite(pfs->queue, pfs->channel, blockno, buffer, 0);
        }
        if (ret != 0) {
            return ret;
        }
        buffer += 32;
        siz_block--;
        cur_block++;
    }
    if (flag == 1 && !(dir.status & 2)) {
        dir.status |= 2;
        if (pfs->activebank != 0) {
            ret = __osPfsSelectBank(pfs, 0);
            if (ret != 0) {
                return ret;
            }
        }
        ret = __osContRamWrite(pfs->queue, pfs->channel, pfs->dir_table + file_no, (u8 *)&dir, 0);
        if (ret != 0) {
            return ret;
        }
    }
    return 0;
}
