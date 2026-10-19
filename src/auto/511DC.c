#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s64 x8; s64 x10; } Unk130518;
extern Unk130518 D_80130518[];
extern s32 D_80097D24[];
extern s32 D_80125C5C;
extern s32 D_80097D48;

void func_800505DC(void) {
    s32 i, j;
    s32 v;
    s32 *p;

    for (j = 0; j < 12; j++) {
        D_80130518[j].x8 = 0;
        D_80130518[j].x0 = 0;
        D_80130518[j].x10 = 0;
    }
    p = D_80097D24;
    v = -1;
    for (i = 3; i >= 0; i--) {
        for (j = 2; j >= 0; j--) {
            p[j] = v;
        }
    }
    D_80097D48 = D_80125C5C = -1;
}
