#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[0x10]; s32 x10; } Obj4679C;
typedef struct { s32 v[4]; } Tbl4679C;
extern s32 D_80097350;
extern Tbl4679C D_800BACB0;
void Timer_Start(void *, s64);
void func_800100BC(s32, s32);

s32 Menu_CycleOption(Obj4679C *obj) {
    s32 next;
    s32 cur;
    Tbl4679C tbl;

    if (obj->x10 == D_80097350 - 1) {
        next = 0;
    } else {
        next = obj->x10 + 1;
    }
    cur = obj->x10;
    tbl = D_800BACB0;
    Timer_Start(obj, 200000000);
    func_800100BC(tbl.v[cur], 0x111F8);
    func_800100BC(tbl.v[next], 0x111F4);
    obj->x10 = next;
    return 1;
}
