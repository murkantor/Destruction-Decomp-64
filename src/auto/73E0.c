#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[0x70 - 0x14];
    u64 x70;
    s32 x78;
} Tracker;
typedef struct { u8 pad[2]; u8 x2; } Unk2C;
typedef struct {
    u8 pad0[0x2C];
    Unk2C *x2C;
    u8 pad30[0x38 - 0x30];
    Tracker *x38;
} Ent;
typedef struct { u8 pad0[0x10]; s32 x10; } Race;
typedef struct {
    u8 pad0[0x10A8];
    u64 x10A8;
    u8 pad10B0[0x1100 - 0x10B0];
    Race *x1100;
} Game;
extern Game *gGame;
extern u32 D_800958B4[];
u64 Time_Get(u64 *);
f32 func_8001FB40(Ent *);

void Ai_CheckStuck(Ent *e) {
    u64 now = Time_Get(&gGame->x10A8);
    Tracker *t = e->x38;
    f32 thresh = 3.0f;

    if (t->x10 == 11 || t->x10 == 5) {
        return;
    }
    if (now < t->x70) {
        return;
    }
    if (now < 7000000000.0) {
        return;
    }
    if (gGame->x1100->x10 == 1 && e->x2C->x2 != 0) {
        return;
    }
    if (D_800958B4[0] >= 4) {
        thresh = 20.0f;
    }
    if (func_8001FB40(e) < thresh) {
        t->x78++;
    } else {
        t->x78 = 0;
    }
    if (t->x78 == 3) {
        f64 d = now + 1500000000.0;
        t->x78 = 0;
        t->x70 = d;
        if (D_800958B4[0] < 4) {
            t->x10 = 5;
        } else {
            t->x10 = 11;
        }
    }
}
