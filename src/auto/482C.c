#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f a; Vec3f b; } Seg;
typedef struct { Vec3f c; f32 r; } Circle;
f32 Vec3f_DistXZ(Vec3f, Vec3f);
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);

Vec3f Seg_AvoidCircleXZ(Seg *s, Circle *c) {
    Vec3f u;
    Vec3f dir;
    Vec3f toC;
    Vec3f p;
    Vec3f res;
    f32 proj;
    f32 sq;
    f32 dAC;
    f32 len;
    f32 disc;
    f32 d;

    toC.x = c->c.x - s->a.x;
    toC.y = 0.0f;
    toC.z = c->c.z - s->a.z;
    dAC = Vec3f_DistXZ(s->a, c->c);
    dir.x = s->b.x - s->a.x;
    dir.y = 0.0f;
    dir.z = s->b.z - s->a.z;
    len = Vec3f_DistXZ(s->a, s->b);
    if (len < dAC && Vec3f_DistXZ(s->b, c->c) > c->r) {
        return s->b;
    }
    u.x = dir.x / len;
    u.y = 0.0f;
    u.z = dir.z / len;
    Vec3f_Dot(&proj, &toC, &u);
    Vec3f_Dot(&sq, &toC, &toC);
    disc = c->r * c->r - (sq - proj * proj);
    if (disc > 0.0f) {
        p.x = s->a.x + u.x * proj;
        p.y = 0.0f;
        p.z = s->a.z + u.z * proj;
        dir.x = p.x - c->c.x;
        dir.y = 0.0f;
        dir.z = p.z - c->c.z;
        len = Vec3f_DistXZ(p, c->c);
        u.x = dir.x / len;
        u.y = 0.0f;
        u.z = dir.z / len;
        d = c->r + 5.0f;
        res.x = c->c.x + u.x * d;
        res.y = 0.0f;
        res.z = c->c.z + u.z * d;
    } else {
        res = s->b;
    }
    return res;
}
