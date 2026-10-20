#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { f32 r, g, b; } Color3f;
typedef struct {
    u8 pad[0x10];
    f32 scale;
    u8 pad14[0x30 - 0x14];
    u16 w;
    u16 h;
    u8 pad34[0x5C - 0x34];
    u8 *x5C;
} Grid;
typedef struct { u8 pad[0xC]; Grid *xC; } TrackX70;
typedef struct { u8 pad[0x70]; TrackX70 *x70; } Track;
typedef struct { u8 pad[0x1100]; Track *track; } Game;
typedef struct { u8 pad[0x10]; Gfx *x10; } Disp;
typedef struct { u8 pad[0x190]; Disp *x190; } Obj;

extern Game *gGame;
#ifdef TARGET_PC
#include "port_sret.h"
#else
void Car_GetPos(Vec3f *, void *);
#endif
void Track_PosToCell(s32 *cx, s32 *cz, f32 x, f32 z);
void Track_CollectCellItems(s32 *out, s32 *count, s32 max, s32 x0, s32 z0, s32 x1, s32 z1);
void Track_FindPolyUnder(s32 *idx, Vec3f *pos, s32 *list, s32 n);
void Track_SampleFaceColor(Color3f *out, f32 x, f32 z, void *tri);

void Car_SetPrimColorFromTrack(Obj *o) {
    Vec3f pos;
    Color3f c;
    s32 list[32];
    s32 cx, cz;
    s32 n;
    s32 idx;
    Gfx *g;

    Car_GetPos(&pos, o);
    Track_PosToCell(&cx, &cz, pos.x * gGame->track->x70->xC->scale, pos.z * gGame->track->x70->xC->scale);
    if (cx >= 0 && cz >= 0 && cx < gGame->track->x70->xC->w && cz < gGame->track->x70->xC->h) {
        Track_CollectCellItems(list, &n, 32, cx, cz, cx, cz);
        Track_FindPolyUnder(&idx, &pos, list, n);
        if (idx != -1) {
            Track_SampleFaceColor(&c, pos.x, pos.z, gGame->track->x70->xC->x5C + idx * 0x18);
            g = o->x190->x10;
            g->w0 = 0xFA000000;
            g->w1 = (((u32)c.r & 0xFF) << 24) | (((u32)c.g & 0xFF) << 16) | (((u32)c.b & 0xFF) << 8);
        }
    }
}
