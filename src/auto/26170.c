#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0xE];
    u8 xE;
    u8 padF;
    s32 x10;
    u8 pad14[4];
    s32 x18;
    u8 pad1C[0x30 - 0x1C];
    s32 x30;
} Car;
typedef struct {
    u8 pad0[4];
    s32 x4;
    s32 x8;
    u8 padC[0x14 - 0xC];
    u8 x14[1];
} Race;
typedef struct {
    u8 pad0[0x10A8];
    u64 x10A8;
    u8 pad10B0[0x10FC - 0x10B0];
    s16 x10FC;
    u8 pad10FE[2];
    Race *x1100;
} Game;
typedef struct {
    Car *x0;
    u8 pad4[0xC];
    u64 x10;
    u8 pad18;
    u8 x19;
    u8 x1A;
    u8 pad1B;
    s32 x1C;
} State;

extern Game *gGame;
extern State *D_80096074;
void Standings_Resort(void *, Car *);
void func_800259A8(Car *);
void Announcer_QueueRandom(s32 *list, s32 n, s32 arg, u8 force, u8 unique);
void Msg_Queue(s32, s32, s32, s32);

void func_80025570(void)
{
    Race *race;
    State *s;
    u64 diff;

    if (gGame->x10FC != 0x1C) {
        return;
    }
    race = gGame->x1100;
    s = D_80096074;
    if (race->x8 == 0) {
        return;
    }
    if (race->x8 == 1 && race->x4 == 0) {
        return;
    }
    if ((s32)((s->x10 - gGame->x10A8) * 1e-9) <= 0 && s->x0 != NULL) {
        s->x0->x10 = 0;
        {
            s32 n = (s32)(s->x10 * 1e-9);
            s->x0->x18 += n * 3;
        }
        Standings_Resort(race->x14, s->x0);
        s->x0->xE = 1;
        s->x1C = 0;
        func_800259A8(s->x0);
    }
    if (gGame->x10FC != 0x1C) {
        return;
    }
    if (s->x0 == NULL) {
        return;
    }
    if (s->x0->x30 != 0) {
        return;
    }
    diff = s->x10 - gGame->x10A8;
    if ((s32)(diff * 1e-9) < 11 && !s->x19) {
        s32 list[2];
        list[0] = 0x69;
        list[1] = 0x74;
        s->x19 = 1;
        Announcer_QueueRandom(list, 2, 0, 0, 0);
        return;
    }
    if ((s32)(diff * 1e-9) < 6 && !s->x1A) {
        s->x1A = 1;
        Msg_Queue(0x6F, 0, 0, 0);
    }
}
