#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s8 x10; } Sub;
typedef struct { u8 pad[0x10]; s32 x10; u8 pad14[0x28 - 0x14]; Sub *x28; } Obj;
extern u8 D_80097344[];
extern s32 D_800958A4;
extern s32 D_800958AC;
void Timer_Start(void *, s64);
void func_80041E44(s32, s32, s32);

s32 func_80041B80(Obj *o) {
    s32 idx;
    s32 prev;

    Timer_Start(o, 300000000LL);
    idx = o->x28->x10;
    if (D_80097344[idx] != 0) {
        return 0;
    }
    if (idx >= D_800958A4) {
        return 0;
    }
    if (o->x10 == 0) {
        prev = 23;
    } else {
        prev = o->x10 - 1;
    }
    func_80041E44(prev, o->x10, idx);
    if (D_800958A4 >= 2 && D_800958AC == 2 && D_800958A4 == 4) {
        switch (idx) {
            case 0:
                func_80041E44(prev, o->x10, 1);
                break;
            case 1:
                func_80041E44(prev, o->x10, 0);
                break;
            case 2:
                func_80041E44(prev, o->x10, 3);
                break;
            case 3:
                func_80041E44(prev, o->x10, 2);
                break;
        }
    }
    o->x10 = prev;
    return 1;
}
