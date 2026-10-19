#include "common.h"
#include "m2c.h"

typedef struct Thing {
    u8 pad[0x28];
    s16 x28;
    s16 x2A;
    s16 *x2C;
    u8 pad30[4];
    s32 x34;
    s32 x38;
    u8 pad3C[4];
    u64 x40;
    u8 x48;
    u8 x49;
    u8 x4A;
    u8 x4B;
    u8 x4C;
    u8 x4D;
    u8 x4E;
    u8 pad4F;
    struct Thing *self;
} Thing;
s16 func_80064D70(void);

void func_800648D4(Thing *t) {
    t->x28 = 20;
    t->x2A = func_80064D70();
    t->x2C = &t->x28;
    t->x40 = 0;
    t->x34 = 2;
    t->x38 = 0;
    t->x49 = 0;
    t->x4A = 0;
    t->x4C = 0;
    t->x4B = 0;
    t->x4E = 0;
    t->x4D = 0;
    t->x48 = 0;
    t->self = t;
}

typedef struct {
    u8 pad[0xC];
    s32 xC;
    s32 x10;
    u8 pad14[4];
    u64 x18;
    u8 pad20[0x48];
} Player;
extern Player D_800958F8[4];

void func_80064948(Player *src) {
    s32 i;

    for (i = 0; i < 4; i++) {
        if (&D_800958F8[i] != src) {
            D_800958F8[i].xC = src->xC;
            D_800958F8[i].x10 = src->x10;
            D_800958F8[i].x18 = src->x18;
        }
    }
}
