#include "common.h"
#include "m2c.h"

void func_80023F60(u8 *obj, s32 value) {
    s16 i = (*(s16 *)(obj + 0xD1C))++;

    ((s32 *)(obj + 0xC10))[i] = value;
    i = (*(s16 *)(obj + 0xD12))++;
    ((s32 *)(obj + 0x40C))[i] = value;
}
