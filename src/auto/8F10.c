#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct {
    u16 x0;
    s16 x2;
    void *x4;
    u8 pad8[4];
    s32 xC;
    u16 x10;
    s32 x14;
    Vec3f x18;
    Vec3f x24;
    f32 x30;
    s32 x34;
    s64 x38;
    s32 x40;
    u8 x44;
    u8 x45;
    s32 x48;
    s32 x4C;
    u8 x50;
} Ent;
typedef struct {
    u16 x0;
    s32 x4;
    u16 x8;
    Vec3f xC;
    Vec3f x18;
    f32 x24;
    f32 x28;
} Rec;
typedef struct { u8 pad[0x1058]; u64 x1058; u8 pad2[0x1080 - 0x1060]; u64 x1080; u8 pad3[0x10FC - 0x1088]; s16 x10FC; } Game;
typedef struct { u8 pad[0x100]; s32 x100; } State;
extern Game *gGame[];
extern State *D_8011EEF8[];
extern s32 D_80132164[];
extern s32 D_801320C8[];
extern s64 D_80131F90[];
extern s16 Obj_NextId(void);
extern u64 Time_Get(u64 *);

Ent *SoundEmitter_Init(Ent *e, Rec *r) {
    f32 f;

    e->x0 = r->x0;
    e->x4 = e;
    e->x2 = Obj_NextId();
    e->x44 = 1;
    e->x45 = 0;
    e->xC = r->x4;
    e->x10 = r->x8;
    e->x18 = r->xC;
    e->x24 = r->x18;
    e->x30 = D_80132164[r->x8] * r->x28;
    f = D_801320C8[r->x8] * r->x24;
    e->x34 = 0;
    e->x40 = 0;
    e->x4C = -1;
    e->x50 = 0;
    e->x14 = f;
    if (gGame[0]->x10FC == 31) {
        e->x38 = (D_80131F90[r->x8] != -1) ? D_80131F90[r->x8] + Time_Get(&gGame[0]->x1080) : -1;
    } else {
        e->x38 = (D_80131F90[r->x8] != -1) ? D_80131F90[r->x8] + Time_Get(&gGame[0]->x1058) : -1;
    }
    e->x48 = 0;
    D_8011EEF8[0]->x100++;
    return e;
}
