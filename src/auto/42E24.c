#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[0xC]; s32 xC; s32 x10; u8 pad14[0x54]; } Player;
typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
    s32 xC;
    s32 x10;
    s32 x14;
    s32 x18;
    s32 x1C[4];
    u8 pad2C[0x28];
    Player p[4];
} Glob;
typedef struct { s32 count; u8 flags[4]; } Slots;
typedef struct { s32 *v[3]; } Tbl;
typedef struct { u8 pad[0x10]; s8 x10; } Info;
typedef struct { u8 pad0[0x10]; s32 x10; u8 pad14[0x14]; Info *x28; } Obj;

extern Glob D_800958A4;
extern Slots D_80097340;
extern Tbl D_800BA5C8;
extern void *gUi[];

void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);
void func_80063564(s32, s32);

s32 func_80042224(Obj *o) {
    Tbl tbl = D_800BA5C8;
    s32 p = o->x28->x10;
    s32 val = o->x10;
    s32 *ids;
    s32 i;
    Glob *g = &D_800958A4;
    Player *pl;
    s32 *q;

    if (p >= D_800958A4.x0) {
        return 0;
    }
    if (D_80097340.flags[p]) {
        D_800958A4.x1C[p] = val;
        D_80097340.flags[p] = 0;
        D_80097340.count--;
        ids = tbl.v[D_800958A4.x0 - 2];
        if (D_800958A4.x0 == 4 && D_800958A4.x8 == 2) {
            switch (p) {
                case 0:
                    D_800958A4.x1C[1] = val;
                    D_80097340.flags[1] = 0;
                    Ui_AddElement(gUi[0], 2, ids[2]);
                    Ui_RemoveElement(gUi[0], ids[3]);
                    break;
                case 1:
                    D_800958A4.x1C[0] = val;
                    D_80097340.flags[0] = 0;
                    Ui_AddElement(gUi[0], 2, ids[0]);
                    Ui_RemoveElement(gUi[0], ids[1]);
                    break;
                case 2:
                    D_800958A4.x1C[3] = val;
                    D_80097340.flags[3] = 0;
                    Ui_AddElement(gUi[0], 2, ids[6]);
                    Ui_RemoveElement(gUi[0], ids[7]);
                    break;
                case 3:
                    D_800958A4.x1C[2] = val;
                    D_80097340.flags[2] = 0;
                    Ui_AddElement(gUi[0], 2, ids[4]);
                    Ui_RemoveElement(gUi[0], ids[5]);
                    break;
            }
        }
        ids += p * 2;
        Ui_AddElement(gUi[0], 2, ids[0]);
        Ui_RemoveElement(gUi[0], ids[1]);
        return 1;
    }
    if (D_800958A4.x0 != 1 && D_80097340.count != 0) {
        return 1;
    }
    pl = g->p;
    q = g->x1C;
    for (i = 0; i < 4; i++) {
        pl[i].xC = 2;
        val = pl[i].x10;
        q[i] = val;
        if (D_80097340.flags[i]) {
            D_80097340.flags[i] = 0;
            D_80097340.count--;
        }
    }
    if (D_800958A4.x4 == 0) {
        func_80063564(0x14, 1);
        o->x10 = D_800958A4.xC;
    } else {
        func_80063564(0x17, 1);
        o->x10 = D_800958A4.x10;
    }
    return 1;
}
