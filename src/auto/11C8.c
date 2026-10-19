#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y;
} V2;

f32 func_800005C8(V2 a, V2 b, V2 c) {
    if (b.x * b.y - a.x * c.y < 0.0) {
        return -1.0f;
    }
    return 1.0f;
}
