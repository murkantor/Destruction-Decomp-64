#include "common.h"
#include "m2c.h"

typedef struct { void *a[4]; } Arr4;
extern Arr4 D_800BACB0;
extern s32 D_80097350;
extern void Timer_Start(void *, s64);
extern void func_800100BC(void *, s32);
typedef struct { u8 pad[0x10]; s32 x10; } Obj;
s32 func_800466D0(Obj *o) {
    s32 prev;
    s32 cur;
    Arr4 tbl;
    if (o->x10 == 0) {
        prev = D_80097350 - 1;
    } else {
        prev = o->x10 - 1;
    }
    cur = o->x10;
    tbl = D_800BACB0;
    Timer_Start(o, 200000000);
    func_800100BC(tbl.a[cur], 0x111F8);
    func_800100BC(tbl.a[prev], 0x111F4);
    o->x10 = prev;
    return 1;
}
