#include "common.h"
#include "m2c.h"

u32 osAiGetLength();                                /* extern */
extern s32 D_800B4E54;

void func_80068260(void) {
    if (((osAiGetLength() >> 2) == 0) && (D_800B4E54 == 0)) {
        D_800B4E54 = 0;
    }
}
