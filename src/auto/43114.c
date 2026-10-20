#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; } Pair;
typedef struct { Pair *p[3]; } Tbl;
typedef struct { u8 pad[0x10]; s8 x10; } Player;
typedef struct { u8 pad[0x10]; s32 x10; u8 pad14[0x28 - 0x14]; Player *x28; } Obj;
typedef struct {
    s32 count;
    s32 mode;
    s32 x8;
    u8 padC[0x1C - 0xC];
    s32 ids[4];
} Sel;

extern Tbl D_800BA5C8;
extern Sel D_800958A4;
extern u8 D_801253F8;
extern u8 D_80097A0C[];
typedef struct { s32 count; u8 flags[4]; } Taken;
extern Taken D_80097340;
extern void *gUi[];
void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);

s32 CarSelect_Confirm(Obj *obj)
{
    Tbl tbl = D_800BA5C8;
    s32 p = obj->x28->x10;
    s32 id = obj->x10;
    Pair *pr;
    s32 i;

    if (p >= D_800958A4.count) {
        return 0;
    }
    if (id != -1 && !D_801253F8 && id >= 12) {
        if (id < 12 || !D_80097A0C[id]) {
            return 0;
        }
    }
    if (D_800958A4.mode == 3) {
        if (D_80097340.flags[p]) {
            return 0;
        }
        if (D_800958A4.x8 == 2 && D_800958A4.count == 4) {
            s32 q = (p + 2) % 4;
            if (D_800958A4.ids[q] == obj->x10 && D_80097340.flags[q] == 1) {
                return 0;
            }
        } else {
            for (i = 0; i < D_800958A4.count; i++) {
                if (D_800958A4.ids[i] == obj->x10 && i != p && D_80097340.flags[i]) {
                    return 0;
                }
            }
        }
    }
    if (D_80097340.flags[p]) {
        return 1;
    }
    D_800958A4.ids[p] = id;
    D_80097340.count++;
    D_80097340.flags[p] = 1;
    if (D_800958A4.count < 2) {
        return 1;
    }
    pr = tbl.p[D_800958A4.count - 2];
    if (D_800958A4.count == 4 && D_800958A4.x8 == 2) {
        switch (p) {
        case 0:
            D_800958A4.ids[1] = id;
            D_80097340.flags[1] = 1;
            Ui_AddElement(gUi[0], 2, pr[1].x4);
            Ui_RemoveElement(gUi[0], pr[1].x0);
            break;
        case 1:
            D_800958A4.ids[0] = id;
            D_80097340.flags[0] = 1;
            Ui_AddElement(gUi[0], 2, pr[0].x4);
            Ui_RemoveElement(gUi[0], pr[0].x0);
            break;
        case 2:
            D_800958A4.ids[3] = id;
            D_80097340.flags[3] = 1;
            Ui_AddElement(gUi[0], 2, pr[3].x4);
            Ui_RemoveElement(gUi[0], pr[3].x0);
            break;
        case 3:
            D_800958A4.ids[2] = id;
            D_80097340.flags[2] = 1;
            Ui_AddElement(gUi[0], 2, pr[2].x4);
            Ui_RemoveElement(gUi[0], pr[2].x0);
            break;
        }
    }
    pr += p;
    Ui_AddElement(gUi[0], 2, pr->x4);
    Ui_RemoveElement(gUi[0], pr->x0);
    return 1;
}
