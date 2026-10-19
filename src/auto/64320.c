#include "common.h"
#include "m2c.h"

typedef struct {
    s32 a;
    s32 b;
} Pair;

Pair func_80063720(u8 *obj) {
    return *(Pair *)(obj + 0x20);
}

u64 Time_Get(u64 *p) {
    return p[0];
}
