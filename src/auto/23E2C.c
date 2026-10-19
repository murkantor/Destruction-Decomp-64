#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
extern Vec3f D_800B8C10;
void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Normalize(Vec3f *, Vec3f *);
void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);

void Camera_UpdateBasis(u8 *arg0) {
    Vec3f up = D_800B8C10;
    Vec3f *a = (Vec3f *)(arg0 + 0x74);
    Vec3f *c = (Vec3f *)(arg0 + 0x80);
    Vec3f *b = (Vec3f *)(arg0 + 0x8C);

    Vec3f_Sub(a, (Vec3f *)(arg0 + 0x54), (Vec3f *)(arg0 + 0x38));
    Vec3f_Normalize(a, a);
    Vec3f_Cross(b, a, &up);
    Vec3f_Normalize(b, b);
    Vec3f_Cross(c, b, a);
    Vec3f_Normalize(c, c);
}
