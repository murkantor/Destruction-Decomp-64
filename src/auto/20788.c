#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} V3;

V3 func_8001FB88(u8 *obj) {
    return *(V3 *)(obj + 0x224);
}
