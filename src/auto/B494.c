#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { void *ptr; u32 size; } HeapBlock;
typedef struct { u8 x0; u8 x1; u8 x2; } Sub;
typedef struct {
    u8 pad[0x2C];
    Sub *x2C;
    u8 pad30[0x9C - 0x30];
    u32 x9C;
    u8 padA0[0xB3C - 0xA0];
    HeapBlock xB3C;
    HeapBlock xB44;
    HeapBlock xB4C;
} Obj;
typedef struct { u8 pad[0x10FC]; s16 x10FC; } Game;
extern Game *gGame;
extern s32 D_800958B4;
void *HeapBlock_Ptr(HeapBlock *);

#define GFX2(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->w0 = (a); _g->w1 = (u32)(b); }

Gfx *func_8000A894(Obj *o, Gfx *g) {
    u8 pushed = 0;
    void *dl;

    if ((o->x2C->x1 == 1 || o->x2C->x2 == 1) && gGame->x10FC == 0x1C) {
        pushed = 1;
        GFX2(g++, 0xDA380000, o->x9C);
        dl = HeapBlock_Ptr(&o->xB3C);
        GFX2(g++, 0xDE000000, dl);
    } else if (D_800958B4 == 2 || D_800958B4 == 11) {
        pushed = 1;
        GFX2(g++, 0xDA380000, o->x9C);
        dl = HeapBlock_Ptr(&o->xB44);
        GFX2(g++, 0xDE000000, dl);
    }
    if (D_800958B4 == 2 || D_800958B4 == 11) {
        dl = HeapBlock_Ptr(&o->xB4C);
        GFX2(g++, 0xDE000000, dl);
    }
    if (pushed) {
        GFX2(g++, 0xD8380002, 0x40);
    }
    return g;
}
