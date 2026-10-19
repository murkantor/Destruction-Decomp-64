#include "common.h"
#include "m2c.h"

typedef struct {
    s32 status;
    OSMesgQueue *queue;
    s32 channel;
    u8 pad[0x50 - 0xC];
    s32 dir_size;
    s32 x54;
    s32 x58;
    s32 dir_table;
} OSPfs;

typedef struct {
    u32 game_code;
    u16 company_code;
    u16 start_page;
    u8 status;
    s8 reserved;
    u16 data_sum;
    u8 ext_name[4];
    u8 game_name[16];
} __OSDir;

s32 __osCheckId(OSPfs *);
s32 __osContRamRead(OSMesgQueue *, int, u16, u8 *);

s32 osPfsFindFile(OSPfs *pfs, u16 company_code, u32 game_code, u8 *game_name, u8 *ext_name, s32 *file_no) {
    s32 j;
    int i;
    __OSDir dir;
    s32 ret = 0;
    int fail;

    if ((pfs->status & 1) == 0) {
        return 5;
    }
    if ((ret = __osCheckId(pfs)) != 0) {
        return ret;
    }
    for (j = 0; j < pfs->dir_size; j++) {
        ret = __osContRamRead(pfs->queue, pfs->channel, (u16)(pfs->dir_table + j), (u8 *)&dir);
        if (ret != 0) {
            return ret;
        }
        if ((dir.company_code == company_code) && (dir.game_code == game_code)) {
            fail = 0;
            if (game_name != 0) {
                for (i = 0; i < 16; i++) {
                    if (dir.game_name[i] != game_name[i]) {
                        fail = 1;
                        break;
                    }
                }
            }
            if ((ext_name != 0) && (fail == 0)) {
                for (i = 0; i < 4; i++) {
                    if (dir.ext_name[i] != ext_name[i]) {
                        fail = 1;
                        break;
                    }
                }
            }
            if (fail == 0) {
                *file_no = j;
                return ret;
            }
        }
    }
    *file_no = -1;
    return 5;
}
