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
    u32 file_size;
    u32 game_code;
    u16 company_code;
    char ext_name[4];
    char game_name[16];
} OSPfsState;

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

s32 __osCheckId(OSPfs *pfs);
s32 __osPfsSelectBank(OSPfs *pfs, u8 bank);
s32 __osContRamRead(OSMesgQueue *mq, int channel, u16 address, u8 *buffer);
s32 __osPfsRWInode(OSPfs *pfs, __OSInode *inode, u8 flag, u8 bank);
void bcopy(const void *src, void *dst, int len);

s32 osPfsFileState(OSPfs *pfs, s32 file_no, OSPfsState *state) {
    s32 ret;
    __OSInode inode;
    __OSDir dir;
    __OSInodeUnit next_page;
    u8 bank;
    s32 pages;

    if (file_no >= pfs->dir_size || file_no < 0) {
        return 5;
    }
    if (!(pfs->status & 1)) {
        return 5;
    }
    if ((ret = __osCheckId(pfs)) != 0) {
        return ret;
    }
    if (pfs->activebank != 0) {
        if ((ret = __osPfsSelectBank(pfs, 0)) != 0) {
            return ret;
        }
    }
    if ((ret = __osContRamRead(pfs->queue, pfs->channel, pfs->dir_table + file_no, (u8 *)&dir)) != 0) {
        return ret;
    }
    if (dir.company_code == 0 || dir.game_code == 0) {
        return 5;
    }
    pages = 0;
    next_page = dir.start_page;
    bank = 0xFF;
    while (1) {
        if (next_page.ipage < pfs->inode_start_page) {
            break;
        }
        if (next_page.inode_t.bank != bank) {
            bank = next_page.inode_t.bank;
            ret = __osPfsRWInode(pfs, &inode, 0, bank);
            if (ret != 0) {
                return ret;
            }
        }
        pages++;
        next_page = inode.inode_page[next_page.inode_t.page];
    }
    if (next_page.ipage != 1) {
        return 3;
    }
    state->file_size = pages * 256;
    state->company_code = dir.company_code;
    state->game_code = dir.game_code;
    bcopy(&dir.game_name, state->game_name, 16);
    bcopy(&dir.ext_name, state->ext_name, 4);
    return 0;
}
