#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s32 x8; } Entry;
typedef struct { u8 pad[0x10]; s32 x10; } Obj;
extern void *gUi;
extern s32 D_80097350;
extern Entry D_80096750[];
extern s32 D_800958A8;
void Timer_Start(void *, s64);
void Ui_RemoveElement(void *, s32);
void Ui_AddElement(void *, s32, s32);

s32 MainMenu_Next(Obj *o) {
    s32 next;
    Entry *tbl;

    Timer_Start(o, 150000000LL);
    tbl = D_80096750;
    if (D_80097350 != 0) {
        next = (o->x10 + 1) % D_80097350;
        if (o->x10 != next) {
            Ui_RemoveElement(gUi, tbl[o->x10].x4);
            Ui_RemoveElement(gUi, tbl[next].x0);
            Ui_AddElement(gUi, 2, tbl[next].x4);
            Ui_AddElement(gUi, 2, tbl[o->x10].x0);
            o->x10 = next;
        }
    }
    D_800958A8 = o->x10;
    return 1;
}
