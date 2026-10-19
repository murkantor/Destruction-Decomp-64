#include "common.h"
#include "m2c.h"

void Handler_Dispatch(s32);

void func_80064304(u8 *obj) {
    s32 i;

    for (i = 0; i < *(s16 *)(obj + 0xD12); i++) {
        Handler_Dispatch(((s32 *)(obj + 0x40C))[i]);
    }
}
