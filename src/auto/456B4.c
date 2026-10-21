#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f a; Vec3f b; Vec3f c; f32 pad[2]; } CamPreset;
extern CamPreset D_80096EE4[12];
extern s32 D_800958B4;
extern f32 D_800D1BA8;

void CourseIntro_GetCamera(Vec3f *a, Vec3f *b, Vec3f *c, f32 *d) {
    s32 i;

    *d = D_800D1BA8;
    i = D_800958B4;
    *a = D_80096EE4[i].a;
    *b = D_80096EE4[i].b;
    *c = D_80096EE4[i].c;
}
