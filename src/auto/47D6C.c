#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Menu;
void Ui_SetElementColor(s32, s32);
void Timer_Start(void *, s64);

s32 ExitConfirm_Toggle(Menu *m) {
    if (m->x10 == 0) {
        Ui_SetElementColor(0x17C, 0x111F8);
        Ui_SetElementColor(0x17D, 0x111F4);
        m->x10 = 1;
    } else {
        Ui_SetElementColor(0x17C, 0x111F4);
        Ui_SetElementColor(0x17D, 0x111F8);
        m->x10 = 0;
    }
    Timer_Start(m, 250000000);
    return 1;
}
