#include "common.h"
#include "m2c.h"

typedef struct { u32 a; u32 b; } HeapBlock;
typedef struct { s32 id; s32 kind; f32 x; f32 y; s32 x10; s32 x14; } UiDef;
typedef struct { u8 pad0[8]; s32 x8; } UiElement;
typedef struct { u8 pad0[0x8B68]; u8 x8B68; } Ui;
typedef struct { u8 x0; u8 pad1[0x27]; } Ent28;
extern Ui *gUi[];
extern HeapBlock D_800BC9F0;
extern HeapBlock D_800BC9E8;
extern s32 D_800BCA08;
extern s32 D_800BCA04;
extern s8 D_800D31CC[];
extern Ent28 D_80125FE8[];
void func_8005DC68(void *);
void func_8005F884(void *);
void *HeapBlock_Ptr(HeapBlock *);
void Ui_AddElement(void *, s32, s32);
UiElement *Ui_FindElement(void *, s32, s32);
void Ui_SetElementPos(void *, s32, s32, f32, f32);
void func_800100BC(s32, s32);

void func_8004378C(void) {
    UiDef *d;
    s32 i;

    func_8005DC68(gUi[0]);
    d = HeapBlock_Ptr(&D_800BC9F0);
    for (i = 0; i < D_800BCA08; d++, i++) {
        Ui_AddElement(gUi[0], d->kind, d->id);
        Ui_FindElement(gUi[0], d->kind, d->id)->x8 = d->x10;
        Ui_SetElementPos(gUi[0], d->kind, d->id, d->x, d->y);
        if (d->kind == 0) {
            func_800100BC(d->id, d->x14);
        }
    }
    func_8005F884(gUi[0]);
    gUi[0]->x8B68 = 0;
    d = HeapBlock_Ptr(&D_800BC9E8);
    for (i = 0; i < D_800BCA04; d++, i++) {
        Ui_AddElement(gUi[0], d->kind, d->id);
        Ui_FindElement(gUi[0], d->kind, d->id)->x8 = d->x10;
        Ui_SetElementPos(gUi[0], d->kind, d->id, d->x, d->y);
        if (d->kind == 0) {
            func_800100BC(d->id, d->x14);
        }
    }
    for (i = 0; i < D_800D31CC[0]; i++) {
        D_80125FE8[i].x0 = 0;
    }
}
