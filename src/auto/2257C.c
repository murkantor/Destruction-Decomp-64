#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3;
typedef struct { u8 pad[0x38]; Vec3 x38; u8 pad2[0x10]; Vec3 x54; } Obj;
extern void Vec3f_Sub(Vec3 *, Vec3 *, Vec3 *);
extern void Vec3f_Scale(Vec3 *, Vec3 *, f32);
extern void Vec3f_Add(Vec3 *, Vec3 *, Vec3 *);
void Vec3f_LerpOffset(Obj *o, Vec3 *a, Vec3 *b, Vec3 *c, f32 t) {
    Vec3 d, p, q;
    Vec3f_Sub(&d, b, a);
    Vec3f_Scale(&d, &d, t);
    p = *a;
    Vec3f_Add(&p, &p, &d);
    q = p;
    Vec3f_Add(&q, &q, c);
    o->x38 = p;
    o->x54 = q;
}
