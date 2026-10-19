#include "common.h"
#include "m2c.h"

typedef struct {
    s32 end;
    u16 *src;
    s32 idx;
    s32 pad;
    u64 interval;
    u64 next;
} Fade;
typedef struct { u16 *x0; } Dst;
typedef struct { u8 pad[0x10]; Dst *x10; u8 pad14[0x88 - 0x14]; s32 x88; } Obj;
typedef struct { u8 pad[0x1058]; u8 x1058[8]; } Game;
extern Game *gGame;
extern Fade D_800D1AE8;
u64 Time_Get(void *);

s32 Fade_Step(Obj *o) {
    s32 i;
    u16 *src;
    u16 *dst;

    if (D_800D1AE8.idx < o->x88) {
        if (Time_Get(gGame->x1058) >= D_800D1AE8.next) {
            for (i = D_800D1AE8.idx, src = D_800D1AE8.src, dst = o->x10->x0; i < D_800D1AE8.end; i += o->x88) {
                dst[i] = src[i];
            }
            D_800D1AE8.next = Time_Get(gGame->x1058) + D_800D1AE8.interval;
            D_800D1AE8.idx++;
        }
    }
    return 0;
}
