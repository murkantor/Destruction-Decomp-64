#include "common.h"
#include "m2c.h"

extern void *gUi;
extern u8 D_800964D0;
void Timer_Start(void *, s64);
void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);

s32 func_8003BACC(void *t) {
    Timer_Start(t, 250000000);
    if (D_800964D0) {
        Ui_RemoveElement(gUi, 3);
        D_800964D0 = 0;
    } else {
        Ui_AddElement(gUi, 2, 3);
        D_800964D0 = 1;
    }
    return 1;
}
