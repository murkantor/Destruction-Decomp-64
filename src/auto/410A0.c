#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
} Entry404A0;

typedef struct {
    u8 pad[0x10];
    s32 x10;
} Obj404A0;

extern void *gUi[];
extern s32 D_80097350[];
extern s32 D_800958A4[];
extern Entry404A0 D_800967B0[];

void Timer_Start(void *, u64);
void Ui_RemoveElement(void *, s32);
void Ui_AddElement(void *, s32, s32);

s32 GameTypeMenu_Prev(Obj404A0 *obj) {
    u8 ok = 0;
    s32 cur;

    Timer_Start(obj, 300000000);
    cur = obj->x10;
    do {
        if (cur == 0) {
            cur = D_80097350[0] - 1;
        } else {
            cur--;
        }
        switch (D_800958A4[0]) {
            case 2:
                ok = 1;
                break;
            case 3:
                if (cur != 2 && cur != 3) {
                    ok = 1;
                }
                break;
            case 4:
                if (cur != 3) {
                    ok = 1;
                }
                break;
        }
    } while (!ok);
    Ui_RemoveElement(gUi[0], D_800967B0[obj->x10].x4);
    Ui_RemoveElement(gUi[0], D_800967B0[cur].x0);
    Ui_AddElement(gUi[0], 2, D_800967B0[cur].x4);
    Ui_AddElement(gUi[0], 2, D_800967B0[obj->x10].x0);
    obj->x10 = cur;
    return 1;
}
