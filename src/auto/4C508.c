#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Menu;
extern s32 D_80097224[];
extern s32 D_80097228[];
void func_800100BC(s32, s32);
void Timer_Start(void *, s64);

s32 func_8004B908(Menu *m) {
    if (m->x10 == 0) {
        m->x10 = 1;
        func_800100BC(D_80097224[0], 0x111F8);
        func_800100BC(D_80097228[0], 0x111F4);
    } else {
        m->x10 = 0;
        func_800100BC(D_80097224[0], 0x111F4);
        func_800100BC(D_80097228[0], 0x111F8);
    }
    Timer_Start(m, 300000000);
    return 1;
}
