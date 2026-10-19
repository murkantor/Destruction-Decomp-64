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

s32 osPfsFindFile(OSPfs *pfs, u16 company_code, u32 game_code, u8 *game_name, u8 *ext_name, s32 *file_no);
s32 __osPfsSelectBank(OSPfs *pfs, u8 bank);
s32 __osContRamRead(OSMesgQueue *mq, s32 channel, u16 address, u8 *buffer);
s32 __osContRamWrite(OSMesgQueue *mq, s32 channel, u16 address, u8 *buffer, s32 force);
s32 __osPfsRWInode(OSPfs *pfs, __OSInode *inode, u8 flag, u8 bank);
s32 __osPfsReleasePages(OSPfs *pfs, __OSInode *inode, u8 start_page, u8 bank, __OSInodeUnit *last_page);
void bzero(void *p, s32 n);

s32 osPfsDeleteFile(OSPfs *pfs, u16 company_code, u32 game_code, u8 *game_name, u8 *ext_name) {
    s32 file_no;
    s32 ret;
    __OSInode inode;
    __OSDir dir;
    __OSInodeUnit last_page;
    u8 startpage;
    u8 bank;

    if (company_code == 0 || game_code == 0) {
        return 5;
    }
    if ((ret = osPfsFindFile(pfs, company_code, game_code, game_name, ext_name, &file_no)) != 0) {
        return ret;
    }
    if (pfs->activebank != 0) {
        if ((ret = __osPfsSelectBank(pfs, 0)) != 0) {
            return ret;
        }
    }
    if ((ret = __osContRamRead(pfs->queue, pfs->channel, (u16)(pfs->dir_table + file_no), (u8 *)&dir)) != 0) {
        return ret;
    }
    startpage = dir.start_page.inode_t.page;
    for (bank = dir.start_page.inode_t.bank; bank < pfs->banks;) {
        if ((ret = __osPfsRWInode(pfs, &inode, 0, bank)) != 0) {
            return ret;
        }
        if ((ret = __osPfsReleasePages(pfs, &inode, startpage, bank, &last_page)) != 0) {
            return ret;
        }
        if ((ret = __osPfsRWInode(pfs, &inode, 1, bank)) != 0) {
            return ret;
        }
        if (last_page.ipage == 1) {
            break;
        }
        bank = last_page.inode_t.bank;
        startpage = last_page.inode_t.page;
    }
    if (bank >= pfs->banks) {
        return 3;
    }
    bzero(&dir, sizeof(__OSDir));
    ret = __osContRamWrite(pfs->queue, pfs->channel, (u16)(pfs->dir_table + file_no), (u8 *)&dir, 0);
    return ret;
}
