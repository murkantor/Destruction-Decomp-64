#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f min; Vec3f max; } Box;
typedef struct { u8 pad[0x10]; s32 n; Vec3f p[1]; } PointList;

void Points_ComputeAABB(PointList *l, Box *b) {
    s32 i;
    f32 ax, nx, ay, ny, az, nz;
    b->max.x = b->min.x = l->p[0].x;
    b->max.y = b->min.y = l->p[0].y;
    b->max.z = b->min.z = l->p[0].z;
    for (i = 1; i < l->n; i++) {
        Vec3f *v = &l->p[i];
        ax = b->max.x;
        if (!(v->x <= ax)) {
            ax = v->x;
        }
        b->max.x = ax;
        nx = b->min.x;
        if (!(nx <= v->x)) {
            nx = v->x;
        }
        b->min.x = nx;
        ay = b->max.y;
        if (!(v->y <= ay)) {
            ay = v->y;
        }
        b->max.y = ay;
        ny = b->min.y;
        if (!(ny <= v->y)) {
            ny = v->y;
        }
        b->min.y = ny;
        az = b->max.z;
        if (!(v->z <= az)) {
            az = v->z;
        }
        b->max.z = az;
        nz = b->min.z;
        if (!(nz <= v->z)) {
            nz = v->z;
        }
        b->min.z = nz;
    }
}
