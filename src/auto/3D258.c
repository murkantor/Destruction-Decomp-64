#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Obj;
extern u8 D_80123C24[];
extern void *func_8001007C(void *);
extern void func_800100BC(void *, s32);
extern void Timer_Start(void *, s64);
s32 func_8003C658(Obj *o) {
    if (o->x10 == 0) {
        o->x10 = 1;
        func_800100BC(func_8001007C(D_80123C24), 0x111F8);
        func_800100BC(func_8001007C(D_80123C24 + 0x32), 0x111F4);
    } else {
        o->x10 = 0;
        func_800100BC(func_8001007C(D_80123C24 + 0x32), 0x111F8);
        func_800100BC(func_8001007C(D_80123C24), 0x111F4);
    }
    Timer_Start(o, 300000000);
    return 1;
}
