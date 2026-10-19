#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10FC]; s16 x10FC; } Game08DD0;
extern Game08DD0 *gGame;
extern s32 D_80124364;
extern u8 D_80124368[];
extern s32 D_80126068[];
extern s32 D_80125F58[];
extern u64 D_801244D8;
extern u8 gGameTime[];
u64 Time_Get(void *);

void Msg_Queue(s32 id, s32 arg1, u8 force, u8 once) {
    s32 i;

    if (D_80124364 < 15) {
        if (gGame->x10FC == 0x19 && id != 0xD2) {
            return;
        }
        if (!force) {
            if (Time_Get(gGameTime) < D_801244D8) {
                return;
            }
        }
        if (once && D_80124368[id]) {
            return;
        }
        i = D_80124364;
        D_80124368[id] = 1;
        D_80126068[i] = id;
        D_80125F58[i] = arg1;
        D_80124364++;
    }
}
