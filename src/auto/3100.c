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
void *Mem_AllocAlignedAlt(u32, u32, u32);
void Mem_Free(void *);
s32 Res_Get(s32);
void Res_Load(s32, void *);
u32 Res_GetSize(s32);

Pair *func_80002500(s32 id) {
    Pair *src = Mem_AllocAlignedAlt(Res_GetSize(id), 7, 0);
    RaceState *r;
    Pair *dst;
    s32 i;

    Res_Load(id, src);
    r = gGame->race;
    dst = Mem_Alloc(r->track->count * sizeof(Pair));
    for (i = 0; i < r->track->count; i++) {
        dst[i].a = src[i].a;
        dst[i].b = Res_Get(src[i].b);
    }
    Mem_Free(src);
    return dst;
}
