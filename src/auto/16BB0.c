#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[8];
    s32 x8;
    s32 xC;
    s32 x10;
    s32 x14;
    s32 x18;
} Info;
typedef struct {
    u8 pad[0xC];
    s32 xC;
    s16 x10;
    s16 x12;
    u8 pad14[4];
    s16 x18;
    s16 x1A;
    u8 pad1C[4];
    void *x20;
    u8 pad24[2];
    s8 x26;
    s8 x27;
    u8 pad28[8];
    s32 x30;
    u8 pad34[0x1C];
    s32 x50;
} Obj;
typedef struct {
    u8 pad[0xA8];
    Obj *xA8;
} RaceState;
typedef struct {
    u8 pad[0x1100];
    RaceState *race;
} GameState;
extern GameState *gGame[];
extern f32 D_80131304[];
void *Mem_AllocAligned(u32, u32, u32);
void Mem_Free(void *);
void *Res_Get(s32);
void Res_Load(s32, void *);

s32 LensFlare_Load(s32 id) {
    Obj *o = Mem_AllocAligned(sizeof(Obj) + 0x14, 0x3F, 0);
    Info *info;
    s32 i;

    Res_Load(id, o);
    info = Res_Get(o->xC);
    o->x50 = 1;
    o->x20 = Res_Get(info->x8);
    o->x12 = info->xC * 4;
    o->x1A = info->x10 * 4;
    o->x26 = info->x14;
    o->x27 = info->x18;
    o->x10 = 0;
    o->x18 = 0;
    o->x30 = 0;
    gGame[0]->race->xA8 = o;
    for (i = 3; i >= 0; i--) {
        D_80131304[i - 3] = 1.0f;
    }
    Mem_Free(info);
    return 0;
}
