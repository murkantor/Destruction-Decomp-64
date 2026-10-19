#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} V3;

V3 func_8001FC08(u8 *obj) {
    return *(V3 *)(obj + 0x23C);
}
