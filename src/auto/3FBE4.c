#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s32 x8; } Pair3EFE4;
typedef struct { u8 pad[0x10]; s32 x10; } Obj3EFE4;
extern void *gUi[];
extern s32 D_80097350;
void Timer_Start(void *, s64);
void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);

void Menu_CycleOption2(Obj3EFE4 *obj, Pair3EFE4 *tbl) {
    s32 n;
    s32 next;

    Timer_Start(obj, 150000000);
    n = D_80097350;
    if (n != 0) {
        next = (obj->x10 + 1) % n;
        if (obj->x10 != next) {
            Ui_RemoveElement(gUi[0], tbl[obj->x10].x4);
            Ui_RemoveElement(gUi[0], tbl[next].x0);
            Ui_AddElement(gUi[0], 2, tbl[next].x4);
            Ui_AddElement(gUi[0], 2, tbl[obj->x10].x0);
            obj->x10 = next;
        }
    }
}
