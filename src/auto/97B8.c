#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10FC]; s16 x10FC; } Game;
extern Game *gGame;
extern s32 D_80124364;
extern u8 D_80124368[];
extern s32 D_80126068[];
extern s32 D_80125F58[];
extern u64 gGameTime;
extern u64 D_801244D8;
u64 Time_Get(u64 *);
int rand(void);

void Announcer_QueueRandom(s32 *list, s32 n, s32 arg, u8 force, u8 unique) {
    s32 id;
    s32 i;
    s32 k;

    if (D_80124364 >= 15) {
        return;
    }
    if (gGame->x10FC == 0x19) {
        return;
    }
    if (!force && Time_Get(&gGameTime) < D_801244D8) {
        return;
    }
    id = -1;
    if (unique) {
        k = rand() % n;
        for (i = 0; i < n; i++) {
            if (!D_80124368[list[k]]) {
                id = list[k];
                break;
            }
            k = (k + 1) % n;
        }
        if (id == -1) {
            return;
        }
    } else {
        k = rand() % n;
        id = list[k];
    }
    if (id < 0) {
        return;
    }
    {
        s32 n = D_80124364;
        D_80124368[id] = 1;
        D_80126068[n] = id;
        D_80125F58[n] = arg;
    }
    D_80124364++;
}
