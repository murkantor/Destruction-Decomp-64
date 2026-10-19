#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[4]; s32 x14; u8 pad8[0xC]; s32 x24; s32 x28; f32 x2C; f32 x30; u8 pad24[0x24]; } UiDesc;
typedef struct { s32 a; s32 b; f32 c; f32 d; } UiPos;
typedef struct { UiPos p[2]; } UiPos2;
typedef struct { s32 x0; s32 x4; s32 x8; s32 xC; s32 x10; s32 x14; } Slot;
extern s32 D_800958A8;
extern s32 D_800958AC;
extern s32 D_800958B4;
extern Slot D_80130518[];
extern UiDesc D_800BAE38;
extern UiPos2 D_800BAE80;
extern void *gUi;
void func_80049340(s32);
void func_8005F050(void *, UiDesc *);
void Ui_AddElement(void *, s32, s32);
void func_80049528(s32, s32, s32, s32);

s32 Menu_NextFreeSlot(s32 cur) {
    s32 side = -100;
    s32 i;
    s32 p;

    if (D_800958A8 == 3) {
        if (D_800958AC == 1) {
            side = 2;
            i = D_800958B4;
            p = -1;
            goto found;
        } else if (D_800958AC == 2) {
            side = 3;
            p = -1;
            i = 12;
            goto found;
        }
    }
    p = !(cur < 4);
    for (i = cur + 1; i < 12; i++) {
        if (D_80130518[i].x0 || D_80130518[i].x8 || D_80130518[i].xC || D_80130518[i].x10 || D_80130518[i].x14) {
            side = !(i < 4);
            break;
        }
    }
    if (i == 12) {
        return -1;
    }
found:
    if (cur == -1) {
        UiDesc d;
        UiPos2 pos;
        func_80049340(side);
        d = D_800BAE38;
        pos = D_800BAE80;
        d.x24 = i + 0x112D0;
        d.x14 = pos.p[0].a;
        d.x28 = pos.p[0].b;
        d.x2C = pos.p[0].c;
        d.x30 = pos.p[0].d;
        func_8005F050(gUi, &d);
        Ui_AddElement(gUi, 2, 0x12E);
        d.x24 = i + 0x112DD;
        d.x14 = pos.p[1].a;
        d.x28 = pos.p[1].b;
        d.x2C = pos.p[1].c;
        d.x30 = pos.p[1].d;
        func_8005F050(gUi, &d);
        Ui_AddElement(gUi, 2, 0x12F);
        p = -1;
    }
    func_80049528(p, side, cur, i);
    return i;
}
