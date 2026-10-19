#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; f32 x10; u8 pad2[8]; f32 x1C; } Rot;
typedef struct { u8 pad[0x38]; Rot *x38; } Holder;
typedef struct { u8 pad[0x10]; s8 x10; } Sub;
typedef struct { u8 pad[0x28]; Sub *x28; } Arg;
typedef struct { u8 pad[0x1058]; u64 x1058; } Game;
extern Holder *D_800970F0;
extern Game *gGame;
extern void *gUi;
extern u64 D_80131298;
extern s32 gNameCursor;
extern u8 D_80123C88[];
extern u64 Time_Get(u64 *);
extern void Ui_RemoveElement(void *, s32);
extern void Ui_AddElement(void *, s32, s32);

s32 NameEntry_UpdateCursor(Arg *arg) {
    u64 step = 500000000;
    Rot *r = D_800970F0->x38;
    f32 diff;
    s32 idx;

    diff = r->x1C - r->x10;
    if (diff > 0.1 || diff < -0.1) {
        if ((diff < 0.0f && diff > -180.0) || diff > 180.0) {
            r->x10 -= 1.3846154f;
        } else {
            r->x10 += 1.3846154f;
        }
        diff = r->x1C - r->x10;
        if ((diff < 0.1 && diff > -0.1) || diff < -359.9) {
            r->x10 = r->x1C;
        } else if (r->x10 < 0.0) {
            r->x10 = r->x10 + 360.0;
        } else if (r->x10 >= 360.0) {
            r->x10 = r->x10 - 360.0;
        }
    }
    if (arg->x28->x10 == 0 && Time_Get(&gGame->x1058) >= D_80131298) {
    if (gNameCursor < 6) {
        idx = gNameCursor + 1;
    } else {
        idx = gNameCursor + 2;
    }
    if (D_80123C88[idx] == '_') {
        Ui_RemoveElement(gUi, 4);
        D_80123C88[idx] = 0;
    } else {
        D_80123C88[idx] = '_';
        Ui_AddElement(gUi, 2, 4);
    }
    D_80131298 += step;
    }
    return 0;
}
