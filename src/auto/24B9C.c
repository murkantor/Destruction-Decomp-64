#include "common.h"
#include "m2c.h"

void func_80023F9C(u8 *obj, s32 value) {
    s16 i;

    ((s32 *)(obj + 0xC))[*(s16 *)(obj + 0xD10)] = value;
    i = (*(s16 *)(obj + 0xD12))++;
    ((s32 *)(obj + 0x40C))[i] = value;
}
