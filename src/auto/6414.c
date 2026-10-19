#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x10];
    u32 x10;
    u8 pad14[0x4C - 0x14];
    s32 x4C;
    s32 x50;
    s32 x54;
    s32 x58;
    u8 pad5C[0x6C - 0x5C];
    u8 x6C;
    u8 x6D;
    u8 pad6E[0x70 - 0x6E];
    u64 x70;
} Tracker;

typedef struct {
    u8 pad0[0x38];
    Tracker *x38;
} Ent;

typedef struct {
    u8 pad0[0x10A8];
    u64 x10A8;
} Game;

extern Game *gGame;
extern Ent **D_80096074;
u64 Time_Get(u64 *);
void Ai_PickTarget(Ent *);
void func_800067E0(Ent *);

void func_80005814(Ent *e) {
    Tracker *t = e->x38;

    switch (t->x10) {
    case 0:
        if (*D_80096074 != e) {
            t->x10 = 1;
        }
        goto tail;
    case 1:
        if (*D_80096074 == e) {
            t->x4C = 4;
            t->x10 = 0;
            t->x50 = 1;
            t->x54 = 2;
            t->x58 = 2;
            Ai_PickTarget(e);
        }
    tail:
        if (t->x6C != 0) {
            t->x10 = 4;
        }
        func_800067E0(e);
        break;
    case 4:
        if (*D_80096074 == e) {
            t->x10 = 0;
            t->x4C = 4;
            t->x50 = 1;
            t->x54 = 2;
            t->x58 = 2;
            Ai_PickTarget(e);
        }
        break;
    case 5:
        if (t->x70 < Time_Get(&gGame->x10A8)) {
            t->x6D = 0;
            t->x10 = 1;
        }
        if (*D_80096074 == e) {
            t->x4C = 4;
            t->x50 = 1;
            t->x10 = 0;
            t->x54 = 2;
            t->x58 = 2;
            t->x6D = 0;
            Ai_PickTarget(e);
        }
        break;
    }
}
