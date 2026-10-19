#include "common.h"
#include "m2c.h"

u8 *func_800653B0(u8 *obj, u8 *src) {
    obj[0xD0] = *src++;
    obj[0xC6] = *src++;
    obj[0xC7] = *src;
    return src + 1;
}
