#include "common.h"
#include "m2c.h"

extern s32 D_800B04A0;

void DebugMenu_Pop(void) {
    if (D_800B04A0 > 0) {
        D_800B04A0 -= 1;
    }
}
