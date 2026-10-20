#include "common.h"
#include "m2c.h"

typedef struct {
    f32 period;
    s8 x4;
    s8 x5;
    u8 pad6[2];
    s32 x8;
    s32 xC;
    f32 acc;
    u8 flag;
    u8 pad15[0x1C - 0x15];
} TimerA;

typedef struct {
    s8 frame;
    u8 pad1;
    s8 numFrames;
    u8 pad3;
    f32 period;
    f32 acc;
    u8 flag;
    u8 padD[0x28 - 0xD];
} TimerB;

typedef struct {
    u8 pad0[0x10A8];
    u64 x10A8;
} Game;

extern Game *gGame;
extern s32 D_80131380;
extern s32 D_80131F28;
extern TimerA D_801255F4[];
extern TimerB D_80124B38[];
f32 Clock_GetDeltaSeconds(u64 *);

void Anim_UpdateTimers(void) {
    f32 dt = Clock_GetDeltaSeconds(&gGame->x10A8);
    s8 i;
    s8 n;
    s8 fr;
    f32 acc;
    f32 period;

    for (i = 0; i < D_80131380; i++) {
        acc = D_801255F4[i].acc + dt;
        period = D_801255F4[i].period;
        D_801255F4[i].acc = acc;
        n = acc / period;
        if (n > 0) {
            D_801255F4[i].flag = 1;
            D_801255F4[i].acc = acc - n * period;
            D_801255F4[i].x8 = D_801255F4[i].x4 * n;
            D_801255F4[i].xC = D_801255F4[i].x5 * n;
        }
    }
    for (i = 0; i < D_80131F28; i++) {
        acc = D_80124B38[i].acc + dt;
        period = D_80124B38[i].period;
        D_80124B38[i].acc = acc;
        n = acc / period;
        if (n > 0) {
            fr = D_80124B38[i].frame + n;
            D_80124B38[i].frame = fr;
            D_80124B38[i].acc = acc - n * period;
            if (fr >= D_80124B38[i].numFrames) {
                D_80124B38[i].frame = 0;
            }
            D_80124B38[i].flag = 1;
        }
    }
}
