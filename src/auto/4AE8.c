#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f p0; Vec3f p1; } Seg;
typedef struct { Vec3f p0; Vec3f p1; s32 x18; } Edge;
typedef struct { s32 count; Edge *items; } EdgeList;

extern u8 Seg_Intersect2D(Seg *seg, Edge *e);
extern f32 func_800005C8(Vec3f a, Vec3f b);
extern f32 Vec3f_AngleBetween(Vec3f a, Vec3f b);
extern void Vec3f_Cross(Vec3f *out, Vec3f *a, Vec3f *b);
extern f32 Vec3f_DistXZ(Vec3f a, Vec3f b);

Vec3f Path_AvoidEdges(Seg *seg, EdgeList *list)
{
    Vec3f d;
    Vec3f e0;
    Vec3f e1;
    Vec3f nrm;
    Edge edge;
    Vec3f v1;
    Vec3f v2;
    Vec3f best;
    Vec3f n;
    Vec3f up = { 0.0f, 10.0f, 0.0f };
    Vec3f result;
    s32 i;
    s32 j;
    s32 hit;
    s32 side;
    s32 side0, side1;
    f32 ang0, ang1, ang;
    f32 len;

    hit = 0;
    for (i = 0; i < list->count; i++) {
        edge = list->items[i];
        hit = Seg_Intersect2D(seg, &edge);
        if (hit) {
            break;
        }
    }
    if (hit) {
        d.x = seg->p1.x - seg->p0.x;
        d.y = 0.0f;
        d.z = seg->p1.z - seg->p0.z;
        e0.x = list->items[i].p0.x - seg->p0.x;
        e0.y = 0.0f;
        e0.z = list->items[i].p0.z - seg->p0.z;
        e1.x = list->items[i].p1.x - seg->p0.x;
        e1.y = 0.0f;
        e1.z = list->items[i].p1.z - seg->p0.z;
        side0 = func_800005C8(d, e0);
        side1 = func_800005C8(d, e1);
        ang0 = Vec3f_AngleBetween(d, e0);
        ang1 = Vec3f_AngleBetween(d, e1);
        for (j = 0; j < i; j++) {
            v1.x = list->items[j].p0.x - seg->p0.x;
            v1.y = 0.0f;
            v1.z = list->items[j].p0.z - seg->p0.z;
            ang = Vec3f_AngleBetween(d, v1);
            side = func_800005C8(d, v1);
            if ((ang0 < ang) & (side0 == side)) {
                ang0 = ang;
                e0 = v1;
            }
        }
        for (j = i + 1; j < list->count; j++) {
            v2.x = list->items[j].p1.x - seg->p0.x;
            v2.y = 0.0f;
            v2.z = list->items[j].p1.z - seg->p0.z;
            ang = Vec3f_AngleBetween(d, v2);
            side = func_800005C8(d, v2);
            if ((ang1 < ang) & (side1 == side)) {
                ang1 = ang;
                e1 = v2;
            }
        }
        if (ang0 < ang1) {
            best = e0;
        } else {
            best = e1;
        }
        if (func_800005C8(d, best) <= 0.0) {
            Vec3f_Cross(&n, &best, &up);
        } else {
            Vec3f_Cross(&n, &up, &best);
        }
        len = Vec3f_DistXZ(best, n);
        nrm.x = n.x / len;
        nrm.y = 0.0f;
        nrm.z = n.z / len;
        result.x = seg->p0.x + best.x + nrm.x * 5.0f;
        result.y = 0.0f;
        result.z = seg->p0.z + best.z + nrm.z * 5.0f;
    } else {
        result = seg->p1;
    }
    return result;
}

/* fills the rest of this file's 8-byte rodata block */
const s32 D_800B7DFC = 0;
