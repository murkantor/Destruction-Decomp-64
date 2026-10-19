#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x1058];
    u64 clock;
} GameState;
extern GameState *gGame;
extern u64 D_80131298;
u64 Time_Get(u64 *);
void func_8004B5C4(void *);

s32 func_8004B6F4(void *o) {
    s32 ret = 0;

    if (Time_Get(&gGame->clock) >= D_80131298) {
        func_8004B5C4(o);
        ret = 1;
    }
    return ret;
}
