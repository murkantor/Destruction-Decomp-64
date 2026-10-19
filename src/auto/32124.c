#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x15]; u8 x15; u8 pad16[2]; } TrackTri;
typedef struct { u8 pad[0x5C]; TrackTri *x5C; } TrackC;
typedef struct { u8 pad[0xC]; TrackC *xC; } Track70;
typedef struct { u8 pad[0x70]; Track70 *x70; } Track;
typedef struct { u8 pad[0x1100]; Track *x1100; } Game;
typedef struct { u8 pad[0x48]; } Face;
extern Game *gGame;
void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Length(f32 *, Vec3f *);
s32 Track_GetCellsInBox(Vec3f *, Vec3f *, s32 *, s32);
void Track_GetFace(Face *, s32);
u8 func_80034794(Face *, f32, Vec3f *, Vec3f *, Vec3f *, Vec3f *, f32 *);

s32 Track_LineOfSight(Vec3f *from, Vec3f *to) {
    Vec3f dir;
    Vec3f start;
    Vec3f end;
    s32 list[0x400];
    f32 len;
    TrackTri *tris;
    s32 steps;
    f32 step;
    s32 i, j, n;
    f32 f;

    tris = gGame->x1100->x70->xC->x5C;
    Vec3f_Sub(&dir, to, from);
    Vec3f_Length(&len, &dir);
    steps = (s32)(len * 0.05) + 1;
    if (steps > 50) steps = 50;
    step = 1.0 / (f32)steps;
    start = *from;
    for (i = 1; i <= steps; i++) {
        f = i * step;
        end.x = from->x + dir.x * f;
        end.y = from->y + dir.y * f;
        end.z = from->z + dir.z * f;
        n = Track_GetCellsInBox(&start, &end, list, 0x400);
        for (j = 0; j < n; j++) {
            Face face;
            Vec3f hitA;
            Vec3f hitB;
            f32 t;

            if (tris[list[j]].x15 & 4) continue;
            Track_GetFace(&face, list[j]);
            if (func_80034794(&face, 0.1f, &start, &end, &hitA, &hitB, &t)) {
                if (t > 0.0 && t < 1.0) return 1;
            }
        }
        start = end;
    }
    return 0;
}
