#include "common.h"
#include "m2c.h"

typedef struct {
    s32 a;
    s32 b;
} Pair;
typedef struct {
    u8 pad[0x14];
    s32 count;
} Track;
typedef struct {
    u8 pad[0x2C];
    Track *track;
} RaceState;
typedef struct {
    u8 pad[0x1100];
    RaceState *race;
} GameState;
extern GameState *gGame;
void *Mem_Alloc(u32);
s32 Res_Get(s32);

Pair *func_80002458(Pair *src) {
    RaceState *r = gGame->race;
    Pair *dst = Mem_Alloc(r->track->count * sizeof(Pair));
    s32 i;

    for (i = 0; i < r->track->count; i++) {
        dst[i].a = src[i].a;
        dst[i].b = Res_Get(src[i].b);
    }
    return dst;
}
