#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[0x18 - 0x14];
    void *x18;
    s32 x1C;
    u8 pad20[0x30 - 0x20];
    s32 x30;
    u8 pad34[0x70 - 0x34];
    u64 x70;
} Tracker;

typedef struct {
    u8 pad0[0xD];
    u8 xD;
    u8 padE[0x38 - 0xE];
    Tracker *x38;
} Ent;

typedef struct {
    u8 pad0[0x10];
    s32 x10;
} Race;

typedef struct {
    u8 pad0[0x10A8];
    u64 x10A8;
    u8 pad10B0[0x1100 - 0x10B0];
    Race *x1100;
} Game;

extern Game *gGame[];
u64 Time_Get(u64 *);
void func_80000F9C(Ent *);

void func_80004620(Ent *a, s32 amt, Ent *b, s32 kind) {
    Tracker *t = b->x38;

    if (a->xD != 0 || b->xD != 0) {
        return;
    }
    if (t->x18 == a) {
        t->x30 += amt;
        if (t->x30 >= t->x1C * 2) {
            t->x30 = 0;
            func_80000F9C(b);
        }
        if (gGame[0]->x1100->x10 == 1 && (kind == 0 || kind == 4 || kind == 1)) {
            t->x70 = Time_Get(&gGame[0]->x10A8) + 3000000000.0;
            t->x10 = 5;
        }
    } else if (t->x1C < 10) {
        t->x30 = 0;
        t->x18 = a;
        t->x1C += 2;
    }
}
