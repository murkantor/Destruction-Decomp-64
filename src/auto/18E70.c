#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct {
    u8 x0;
    u8 pad1[0x27];
    s32 x28;
} Seg;
typedef struct {
    u8 x0;
    u8 pad1[3];
    s32 x4;
    u8 pad8[0x21C];
    Seg *x224;
    u8 pad228[4];
    Vec3f cur;
    Vec3f prev;
    Vec3f x244;
    Vec3f x250;
    Vec3f x25C[4];
    f32 x28C[4];
    Vec3f x29C;
    f32 x2A8;
} Skid;
typedef struct { u8 pad0[0x10]; Vec3f up; } Body;
typedef struct { u8 pad0[0x30]; Vec3f pos; } Mtx;
typedef struct { u8 pad0[0x30]; u16 x30; u16 x32; } Grid;
typedef struct { u8 pad0[0xC]; Grid *xC; } TrackInfo;
typedef struct { u8 pad0[0x70]; TrackInfo *x70; } Race;
typedef struct { u8 pad0[0x1100]; Race *race; } Game;
typedef struct { u8 pad0[0xC]; f32 xC; } Scale;
extern Game *gGame;
extern Scale *D_800BC9C0;
void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_SetLength(Vec3f *, Vec3f *, f32);
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);
void Points_BoundsXZ(f32 *, f32 *, f32 *, f32 *, Vec3f *, s32);
void Track_PosToCell(s32 *, s32 *, f32, f32);
void Track_CollectCellItems(s32 *, s32 *, s32, s32, s32, s32, s32);
void Skid_ClipPolys(Skid *, s32 *, s32);

void Skid_Update(Skid *s, Body *b, Mtx *m) {
    Vec3f q[4];
    s32 items[40];
    Vec3f v[3];
    f32 minX;
    f32 maxX;
    f32 minZ;
    f32 maxZ;
    s32 cx0;
    s32 cz0;
    s32 cx1;
    s32 cz1;
    s32 count;
    s32 i;
    s32 j;
    s32 k;

    for (k = 0; k < 40; k++) {
        if (s->x224[k].x0) {
            if (s->x224[k].x28 > 0) {
                s->x224[k].x28--;
            } else {
                s->x224[k].x0 = 0;
            }
        }
    }
    s->cur.x = m->pos.x;
    s->cur.y = m->pos.y;
    s->cur.z = m->pos.z;
    s->x4++;
    if (!s->x0 || s->x4 == 1) {
        s->prev = s->cur;
        return;
    }
    Vec3f_Sub(&v[1], &s->cur, &s->prev);
    v[2].x = b->up.x;
    v[2].y = b->up.y;
    v[2].z = b->up.z;
    Vec3f_Cross(&v[0], &v[1], &v[2]);
    Vec3f_SetLength(&v[0], &v[0], 1.0f);
    v[0].x *= 0.2;
    v[0].y *= 0.2;
    v[0].z *= 0.2;
    if (s->x4 == 2) {
        q[0].x = s->prev.x + v[0].x;
        q[0].y = s->prev.y + v[0].y;
        q[0].z = s->prev.z + v[0].z;
        q[1].x = s->prev.x - v[0].x;
        q[1].y = s->prev.y - v[0].y;
        q[1].z = s->prev.z - v[0].z;
    } else {
        q[0] = s->x244;
        q[1] = s->x250;
    }
    q[2].x = s->cur.x - v[0].x;
    q[2].y = s->cur.y - v[0].y;
    q[2].z = s->cur.z - v[0].z;
    q[3].x = s->cur.x + v[0].x;
    q[3].y = s->cur.y + v[0].y;
    q[3].z = s->cur.z + v[0].z;
    for (i = 0; i < 4; i++) {
        j = (i + 1) % 4;
        v[0].x = q[j].x - q[i].x;
        v[0].y = q[j].y - q[i].y;
        v[0].z = q[j].z - q[i].z;
        Vec3f_Cross(&s->x25C[i], &v[0], &v[2]);
        Vec3f_Dot(&s->x28C[i], &q[i], &s->x25C[i]);
    }
    s->x29C = v[2];
    if (0.0f < s->x29C.y) {
        s->x29C.x = -s->x29C.x;
        s->x29C.y = -s->x29C.y;
        s->x29C.z = -s->x29C.z;
    }
    Vec3f_Dot(&s->x2A8, &s->cur, &s->x29C);
    Points_BoundsXZ(&minX, &maxX, &minZ, &maxZ, q, 4);
    minX *= D_800BC9C0->xC;
    minZ *= D_800BC9C0->xC;
    maxX *= D_800BC9C0->xC;
    maxZ *= D_800BC9C0->xC;
    Track_PosToCell(&cx0, &cz0, minX, minZ);
    Track_PosToCell(&cx1, &cz1, maxX, maxZ);
    if (cx0 < 0) {
        cx0 = 0;
    }
    if (cz0 < 0) {
        cz0 = 0;
    }
    if (cx1 > gGame->race->x70->xC->x30 - 1) {
        cx1 = gGame->race->x70->xC->x30 - 1;
    }
    if (cz1 > gGame->race->x70->xC->x32 - 1) {
        cz1 = gGame->race->x70->xC->x32 - 1;
    }
    if (cx0 <= cx1 && cz0 <= cz1 && (s->cur.x != s->prev.x || s->cur.y != s->prev.y || s->cur.z != s->prev.z)) {
        Track_CollectCellItems(items, &count, 40, cx0, cz0, cx1, cz1);
    } else {
        count = 0;
    }
    Skid_ClipPolys(s, items, count);
    s->prev = s->cur;
    s->x244 = q[3];
    s->x250 = q[2];
}
