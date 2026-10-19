#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Menu;
extern void *gUi[];
void func_800100BC(s32, s32);
void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);
void Timer_Start(void *, s64);

s32 func_8003D858(Menu *m) {
    if (m->x10 == 2) {
        func_800100BC(0x183, 0x111F8);
        m->x10 = 0;
        func_800100BC(0x181, 0x111F4);
        Ui_RemoveElement(gUi[0], 0x130);
        Ui_AddElement(gUi[0], 2, 0x131);
    } else {
        func_800100BC(m->x10 + 0x181, 0x111F8);
        m->x10++;
        func_800100BC(m->x10 + 0x181, 0x111F4);
        if (m->x10 == 1) {
            Ui_RemoveElement(gUi[0], 0x131);
            Ui_AddElement(gUi[0], 2, 0x130);
        }
    }
    Timer_Start(m, 250000000);
    return 1;
}
