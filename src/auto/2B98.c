#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x30]; void *x30; u8 pad34[4]; u32 x38; } Track;
typedef struct { u8 pad[0x1100]; Track *x1100; } Game;
extern Game *gGame;
u32 Res_GetSize(s32);
void *Mem_AllocAligned(u32, s32, s32);
void Res_Load(s32, void *);

void *Track_LoadNodeSegs(s32 id) {
    u32 size = Res_GetSize(id);
    void *p = Mem_AllocAligned(size, 7, 0);
    u32 n = size / 24;
    Track *t;

    Res_Load(id, p);
    PORT_CONVERT(p, n * 6, 4); /* two Vec3f each, passed by value */
    t = gGame->x1100;
    t->x30 = p;
    t->x38 = n;
    return p;
}
