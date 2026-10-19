#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10A8]; u64 x10A8; } Game;
typedef struct {
    u8 pad0[0x30];
    s32 x30;
    s32 x34;
    u8 pad38[0x60 - 0x38];
    u64 x60[4];
    u64 x80;
    s32 x88;
    s32 x8C;
} Racer;
typedef struct { u64 x0; u8 pad8[0x38 - 8]; } Rec56;
typedef struct { u64 x0; u8 pad8[0x18 - 8]; } Rec24;
typedef struct { s32 v[3]; } List3;

extern Game *gGame;
extern s32 D_800958A4;
extern s32 D_800958A8;
extern s32 D_800958AC;
extern s32 D_800958B4;
extern s32 D_800D1C20[];
extern s32 D_80095850[];
extern Rec56 D_80097A50[];
extern Rec24 D_80130520[];
extern List3 D_800B8A58;
extern u8 D_80097335;
extern s32 D_80097D48;
u64 Time_Get(u64 *);
void Announcer_QueueRandom(s32 *list, s32 n, s32 arg, u8 force, u8 unique);
void func_8002A8D8(Racer *);

void Racer_UpdateLapTiming(Racer *o) {
    s32 i;
    s32 lim;
    u64 now;
    u64 lap;

    o->x8C++;
    if ((o->x8C & 3) == 0 && D_800958B4 == 8) {
        s32 id = o->x34;
        if (D_800958A8 == 3 && D_800958AC == 2) {
            i = id;
        } else {
            for (i = 0; i < 12; i++) {
                if (D_800D1C20[i] == id) {
                    break;
                }
            }
        }
        if (D_80095850[i] != 0) {
            D_80095850[i] = 0;
        } else {
            D_80095850[i] = 1;
        }
    }
    if (o->x8C == 4 && o->x88 == -1) {
        o->x88 = 0;
        return;
    }
    if (D_800958B4 == 8) {
        lim = o->x88 * 8 + 12;
    } else {
        lim = (o->x88 + 2) * 4;
    }
    if (o->x8C != lim) {
        return;
    }
    if (++o->x88 <= 0) {
        return;
    }
    now = Time_Get(&gGame->x10A8);
    lap = now - o->x60[0];
    *(u64 *)((u8 *)o + (o->x88 << 3) + 0x60) = lap;
    if (o->x80 == 0) {
        o->x80 = lap;
    } else {
        o->x80 = (lap < o->x80) ? lap : o->x80;
    }
    o->x60[0] = now;
    if (o->x30 == 0 && D_800958A4 == 1) {
        s32 m = D_800958B4;
        if (lap < D_80097A50[m].x0) {
            List3 list = D_800B8A58;
            if (D_80130520[m].x0 == 0) {
                D_80130520[m].x0 = lap;
                Announcer_QueueRandom(list.v, 3, 0, 1, 0);
            } else if (lap < D_80130520[m].x0) {
                D_80130520[m].x0 = lap;
                Announcer_QueueRandom(list.v, 3, 0, 1, 0);
            }
            D_80097335 = 1;
            D_80097D48 = 0;
        }
    }
    func_8002A8D8(o);
}
