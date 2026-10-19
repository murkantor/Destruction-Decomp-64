#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x0;
    f32 x4;
    f32 x8;
    f32 xC;
    f32 m[3][3];
    s32 x34;
    s32 x38;
    s32 x3C;
    s32 x40;
    s32 x44;
    u8 pad48[4];
} Cam;
typedef struct {
    u8 pad0[0x38];
    Cam *x38;
} Obj;
typedef struct {
    f32 x, y, z;
} Vec3;
void *Mem_Alloc(s32);
void func_8003AAC4(void *);

void Cam_Create(Obj *obj, Vec3 v) {
    Cam *cam;
    s32 i;

    cam = Mem_Alloc(0x4C);
    cam->x0 = 0.5f;
    cam->x4 = v.x;
    cam->x8 = v.y;
    cam->xC = v.z;
    cam->x34 = 0;
    cam->x38 = 0;
    cam->x3C = 0;
    cam->x40 = 0;
    cam->x44 = 0;
    cam->x40 = 0;
    for (i = 0; i < 3; i++) {
        if (i == 0) {
            cam->m[0][0] = 1.0f;
        } else {
            cam->m[i][0] = 0.0f;
        }
        if (i == 1) {
            cam->m[1][1] = 1.0f;
        } else {
            cam->m[i][1] = 0.0f;
        }
        if (i == 2) {
            cam->m[2][2] = 1.0f;
        } else {
            cam->m[i][2] = 0.0f;
        }
    }
    func_8003AAC4(cam->m);
    obj->x38 = cam;
}
