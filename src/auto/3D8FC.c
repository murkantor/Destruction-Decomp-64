#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Menu;
extern s32 D_80097350[];
void Ui_SetElementColor(s32, s32);
void Timer_Start(void *, s64);

s32 func_8003CCFC(Menu *m) {
    Ui_SetElementColor(m->x10 + 0x17C, 0x111F8);
    m->x10++;
    if (m->x10 >= D_80097350[0]) {
        m->x10 = 0;
    }
    Ui_SetElementColor(m->x10 + 0x17C, 0x111F4);
    Timer_Start(m, 250000000);
    return 1;
}
