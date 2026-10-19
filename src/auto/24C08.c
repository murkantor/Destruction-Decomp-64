#include "common.h"
#include "m2c.h"

void func_80024008(u8 *obj, s32 value) {
    s16 i = (*(s16 *)(obj + 0xD18))++;

    ((s32 *)(obj + 0xA90))[i] = value;
}
