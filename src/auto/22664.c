#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x1A]; u8 x1A; } CamCfg;
typedef struct {
    u8 pad0[0x38];
    Vec3f x38;
    u8 pad1[0x54 - 0x44];
    Vec3f x54;
    u8 pad2[0x68 - 0x60];
    Vec3f x68;
    u8 pad3[0x98 - 0x74];
    Vec3f x98;
    u8 pad4[0xB0 - 0xA4];
    CamCfg *xB0;
} Cam;
typedef struct {
    u8 pad0[0x290];
    Vec3f x290;
    Vec3f x29C;
    Vec3f x2A8;
    Vec3f x2B4;
    Vec3f x2C0;
    Vec3f x2CC;
} Car;
extern void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);
extern void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
extern void Camera_UpdateBasis(Cam *);

void func_80021A64(Cam *cam, Car *car, f32 fz, f32 fx, f32 fy, f32 tx, f32 ty) {
    Vec3f pos;
    Vec3f tmp;
    f32 nx;
    Vec3f *eye;

    pos = car->x290;
    eye = &cam->x38;
    if (cam->xB0->x1A) {
        fx = -fx;
        tx = -tx;
    }
    nx = -fx;
    cam->x38.x = pos.x + nx * car->x2B4.x + fy * car->x2C0.x + fz * car->x2CC.x;
    cam->x38.y = pos.y + nx * car->x2B4.y + fy * car->x2C0.y + fz * car->x2CC.y;
    cam->x38.z = pos.z + nx * car->x2B4.z + fy * car->x2C0.z + fz * car->x2CC.z;
    cam->x54.x = pos.x + tx * car->x2B4.x + ty * car->x2C0.x;
    cam->x54.y = pos.y + tx * car->x2B4.y + ty * car->x2C0.y;
    cam->x54.z = pos.z + tx * car->x2B4.z + ty * car->x2C0.z;
    Vec3f_Cross(&tmp, &car->x2A8, eye);
    Vec3f_Add(&tmp, &tmp, &car->x29C);
    cam->x98 = tmp;
    cam->x68 = car->x2C0;
    Camera_UpdateBasis(cam);
}
