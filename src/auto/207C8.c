#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} V3;

void func_8001FBC8(u8 *obj, V3 v) {
    *(V3 *)(obj + 0x218) = v;
    *(V3 *)(obj + 0x290) = v;
}
