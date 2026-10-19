#include "common.h"
#include "m2c.h"

extern s32 D_8007EE54;
void func_80007FD4(s32 arg0, u8 *obj) {
    u8 *p;
    if (*(s32 *)(obj + 0x30) != 0) return;
    p = *(u8 **)(obj + 0x3C);
    if (*(s32 *)(p + 0x60) == 0) {
        *(s32 *)(p + 0x5C) = D_8007EE54;
    } else {
        *(s32 *)(p + 0x5C) = 0;
    }
}
