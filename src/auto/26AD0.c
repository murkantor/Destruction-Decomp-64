#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct {
    f32 x0; f32 x4; f32 x8; f32 xC; f32 x10; f32 m[3][3];
    s32 x38; s32 x3C; s32 x40; s32 x44; s32 x48; u8 pad4C[4];
} Cam;
typedef struct { u8 pad[0x38]; Cam *x38; } Owner;
void *Mem_Alloc(u32 size);
void Basis_Orthonormalize(f32 (*)[3]);

void Prop_Create(Owner *o, Vec3f pos) {
    Cam *cam = Mem_Alloc(0x50);
    s32 i;

    cam->x0 = -1.0f;
    cam->x4 = 1.5f;
    cam->x8 = pos.x;
    cam->xC = pos.y;
    cam->x10 = pos.z;
    cam->x38 = 0;
    cam->x3C = 0;
    cam->x40 = 0;
    cam->x44 = 0;
    cam->x48 = 0;
    cam->x44 = 0;
    for (i = 0; i < 3; i++) {
        if (i == 0) {
            cam->m[i][0] = 1.0f;
        } else {
            cam->m[i][0] = 0.0f;
        }
        if (i == 1) {
            cam->m[i][1] = 1.0f;
        } else {
            cam->m[i][1] = 0.0f;
        }
        if (i == 2) {
            cam->m[i][2] = 1.0f;
        } else {
            cam->m[i][2] = 0.0f;
        }
    }
    Basis_Orthonormalize(cam->m);
    o->x38 = cam;
}
