#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0xC];
    s32 xC;
} Entry;
typedef struct {
    u8 pad[0x38];
    s32 x38;
    Entry *x3C;
    u32 x40;
} RaceState;
typedef struct {
    u8 pad[0x1100];
    RaceState *race;
} GameState;
extern GameState *gGame;
u32 Res_GetSize(s32);
void *Mem_AllocAligned(u32, u32, u32);
void Res_Load(s32, void *);

Entry *Track_LoadBranches(s32 id) {
    u32 size = Res_GetSize(id);
    Entry *e = Mem_AllocAligned(size, 7, 0);
    RaceState *r;
    u32 n;
    s32 i;
    char unused[1];

    Res_Load(id, e);
    r = gGame->race;
    n = size >> 4;
    r->x3C = e;
    r->x40 = n;
    for (i = 0; i < (s32)n; i++) {
        r->x38 -= e->xC;
    }
    return e;
}
