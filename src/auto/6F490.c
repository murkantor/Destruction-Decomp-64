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

typedef struct {
    __OSInodeUnit map[256];
    u8 bank;
} __OSInodeCache;

extern s32 __osCheckId(OSPfs *);
extern s32 __osGetId(OSPfs *);
extern s32 __osPfsRWInode(OSPfs *, __OSInode *, u8, u8);
extern s32 __osPfsSelectBank(OSPfs *, u8);
extern s32 __osContRamWrite(OSMesgQueue *, s32, u16, u8 *, s32);
extern s32 __osContRamRead(OSMesgQueue *, s32, u16, u8 *);
extern s32 corrupted_init(OSPfs *, __OSInodeCache *);
extern s32 corrupted(OSPfs *, __OSInodeUnit, __OSInodeCache *);
extern void bzero(void *, s32);

#define CHECK_IPAGE(p) (((p).ipage >= pfs->inode_start_page) && ((p).inode_t.bank < pfs->banks) && ((p).inode_t.page >= 0x01) && ((p).inode_t.page < 0x80))

s32 osPfsChecker(OSPfs *pfs) {
    int j;
    s32 ret;
    __OSInodeUnit next_page;
    __OSInode checked_inode;
    __OSInode tmp_inode;
    __OSDir tmp_dir;
    __OSInodeUnit file_next_node[16];
    __OSInodeCache cache;
    int fixed;
    u8 bank, oldbank;
    int offset;
    s32 cc, cl;

    fixed = 0;
    oldbank = 254;
    ret = __osCheckId(pfs);
    if (ret == 2) {
        ret = __osGetId(pfs);
    }
    if (ret != 0) {
        return ret;
    }
    ret = corrupted_init(pfs, &cache);
    if (ret != 0) {
        return ret;
    }
    for (j = 0; j < pfs->dir_size; j++) {
        ret = __osContRamRead(pfs->queue, pfs->channel, pfs->dir_table + j, (u8 *)&tmp_dir);
        if (ret != 0) {
            return ret;
        }
        if (tmp_dir.company_code != 0 || tmp_dir.game_code != 0) {
            if (tmp_dir.company_code == 0 || tmp_dir.game_code == 0) {
                cc = -1;
            } else {
                next_page = tmp_dir.start_page;
                cl = cc = 0;
                bank = 255;
                while (CHECK_IPAGE(next_page)) {
                    if (bank != next_page.inode_t.bank) {
                        bank = next_page.inode_t.bank;
                        if (oldbank != bank) {
                            ret = __osPfsRWInode(pfs, &tmp_inode, 0, bank);
                            oldbank = bank;
                        }
                        if (ret != 0 && ret != 3) {
                            return ret;
                        }
                    }
                    cc = corrupted(pfs, next_page, &cache) - cl;
                    if (cc != 0) {
                        break;
                    }
                    cl = 1;
                    next_page = tmp_inode.inode_page[next_page.inode_t.page];
                }
            }
            if (cc != 0 || next_page.ipage != 1) {
                bzero(&tmp_dir, sizeof(__OSDir));
                if (pfs->activebank != 0) {
                    ret = __osPfsSelectBank(pfs, 0);
                    if (ret != 0) {
                        return ret;
                    }
                }
                ret = __osContRamWrite(pfs->queue, pfs->channel, pfs->dir_table + j, (u8 *)&tmp_dir, 0);
                if (ret != 0) {
                    return ret;
                }
                fixed++;
            }
        }
    }
    for (j = 0; j < pfs->dir_size; j++) {
        ret = __osContRamRead(pfs->queue, pfs->channel, pfs->dir_table + j, (u8 *)&tmp_dir);
        if (ret != 0) {
            return ret;
        }
        if (tmp_dir.company_code != 0 && tmp_dir.game_code != 0 && tmp_dir.start_page.ipage >= (u16)pfs->inode_start_page) {
            file_next_node[j].ipage = tmp_dir.start_page.ipage;
        } else {
            file_next_node[j].ipage = 0;
        }
    }
    for (bank = 0; bank < pfs->banks; bank++) {
        ret = __osPfsRWInode(pfs, &tmp_inode, 0, bank);
        if (ret != 0 && ret != 3) {
            return ret;
        }
        offset = (bank > 0) ? 1 : pfs->inode_start_page;
        for (j = 0; j < offset; j++) {
            checked_inode.inode_page[j].ipage = tmp_inode.inode_page[j].ipage;
        }
        for (; j < 128; j++) {
            checked_inode.inode_page[j].ipage = 3;
        }
        for (j = 0; j < pfs->dir_size; j++) {
            while (file_next_node[j].inode_t.bank == bank && file_next_node[j].ipage >= (u16)pfs->inode_start_page) {
                u8 pp = file_next_node[j].inode_t.page;
                file_next_node[j] = checked_inode.inode_page[pp] = tmp_inode.inode_page[pp];
            }
        }
        ret = __osPfsRWInode(pfs, &checked_inode, 1, bank);
        if (ret != 0) {
            return ret;
        }
    }
    if (fixed) {
        pfs->status |= 2;
    } else {
        pfs->status &= ~2;
    }
    return 0;
}
