#include "common.h"
#include "m2c.h"

typedef struct {
    int status;
    OSMesgQueue *queue;
    int channel;
    u8 id[32];
    u8 label[32];
    int version;
    int dir_size;
    int inode_table;
    int minode_table;
    int dir_table;
    int inode_start_page;
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
s32 osPfsFreeBlocks(OSPfs *pfs, s32 *bytes_not_used);
s32 __osPfsRWInode(OSPfs *pfs, __OSInode *inode, u8 flag, u8 bank);
s32 __osPfsDeclearPage(OSPfs *pfs, __OSInode *inode, int file_size_in_pages, int *first_page, u8 bank, int *decleared, int *last_page);
s32 __osContRamWrite(OSMesgQueue *mq, int channel, u16 address, u8 *buffer, int force);
void bcopy(const void *src, void *dst, int len);

s32 osPfsAllocateFile(OSPfs *pfs, u16 company_code, u32 game_code, u8 *game_name, u8 *ext_name, int file_size_in_bytes, s32 *file_no)
{
    int start_page, decleared, last_page, old_last_page;
    __OSInode inode, backup_inode;
    __OSDir dir;
    u8 bank, prevbank;
    s32 ret;
    int firsttime;
    s32 bytes;
    __OSInodeUnit fpage;
    int file_size_in_pages;

    old_last_page = 0;
    prevbank = 0;
    firsttime = 0;
    if (company_code == 0 || game_code == 0)
        return 5;
    file_size_in_pages = (file_size_in_bytes + 255) / 256;
    if ((ret = osPfsFindFile(pfs, company_code, game_code, game_name, ext_name, file_no)) != 0 && ret != 5)
        return ret;
    if (*file_no != -1)
        return 9;
    ret = osPfsFreeBlocks(pfs, &bytes);
    if (file_size_in_bytes > bytes)
        return 7;
    if (file_size_in_pages == 0)
        return 5;
    if ((ret = osPfsFindFile(pfs, 0, 0, 0, 0, file_no)) != 0 && ret != 5)
        return ret;
    if (*file_no == -1)
        return 8;
    for (bank = 0; bank < pfs->banks; bank++) {
        if ((ret = __osPfsRWInode(pfs, &inode, 0, bank)) != 0)
            return ret;
        if ((ret = __osPfsDeclearPage(pfs, &inode, file_size_in_pages, &start_page, bank, &decleared, &last_page)) != 0)
            return ret;
        if (start_page != -1) {
            if (firsttime == 0) {
                fpage.inode_t.page = (u8)start_page;
                fpage.inode_t.bank = bank;
            } else {
                backup_inode.inode_page[old_last_page].inode_t.bank = bank;
                backup_inode.inode_page[old_last_page].inode_t.page = (u8)start_page;
                if ((ret = __osPfsRWInode(pfs, &backup_inode, 1, prevbank)) != 0)
                    return ret;
            }
            if (file_size_in_pages > decleared) {
                bcopy(&inode, &backup_inode, sizeof(__OSInode));
                old_last_page = last_page;
                prevbank = bank;
                file_size_in_pages -= decleared;
                firsttime++;
            } else {
                file_size_in_pages = 0;
                if ((ret = __osPfsRWInode(pfs, &inode, 1, bank)) != 0)
                    return ret;
                break;
            }
        }
    }
    if (file_size_in_pages > 0 || start_page == -1)
        return 3;
    dir.start_page = fpage;
    dir.company_code = company_code;
    dir.game_code = game_code;
    dir.data_sum = 0;
    bcopy(game_name, dir.game_name, 16);
    bcopy(ext_name, dir.ext_name, 4);
    ret = __osContRamWrite(pfs->queue, pfs->channel, pfs->dir_table + *file_no, (u8 *)&dir, 0);
    return ret;
}
