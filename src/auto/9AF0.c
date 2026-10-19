#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10FC]; s16 x10FC; } Game;

extern s32 D_80124364;
extern s32 D_8013137C;
extern s32 D_80125C18;
extern u64 D_801244D8;
extern u8 D_80124368[];
extern Game *gGame;
extern u64 gGameTime;

u64 Time_Get(u64 *);
int rand(void);

void func_80008EF0(s32 *list, s32 n, s32 arg2, u8 arg3) {
    s32 sel;
    s32 i;
    s32 idx;

    if (D_80124364 > 0) {
        return;
    }
    if (D_8013137C >= 0) {
        return;
    }
    if (Time_Get(&gGameTime) < D_801244D8) {
        return;
    }
    if (gGame->x10FC == 0x19) {
        return;
    }
    sel = -1;
    if (arg3) {
        idx = rand() % n;
        for (i = 0; i < n; i++) {
            if (D_80124368[list[idx]] == 0) {
                sel = list[idx];
                break;
            }
            idx = (idx + 1) % n;
        }
        if (sel == -1) {
            return;
        }
    } else {
        idx = rand() % n;
        sel = list[idx];
    }
    D_8013137C = sel;
    D_80125C18 = arg2;
}
