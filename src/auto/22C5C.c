#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} Vec3;
typedef struct {
    u8 pad0[0x38];
    f32 x38;
    f32 x3C;
    f32 x40;
    u8 pad44[0x10];
    Vec3 x54;
} Cam;
typedef struct {
    u8 pad0[0x290];
    Vec3 x290;
} Car;
extern f64 D_800B8B50;
extern f64 D_800B8B58;
extern f64 D_800B8B60;
extern f32 D_80095624;
void Math_SinCos(f32, f32 *, f32 *);
void Camera_UpdateBasis(Cam *);

void func_8002205C(Cam *cam, Car *car, f32 speed, f32 *angle) {
    f32 s;
    f32 c;

    *angle -= (f32)(speed * D_800B8B50);
    Math_SinCos(*angle, &s, &c);
    cam->x54 = car->x290;
    cam->x54.y += D_800B8B58;
    cam->x38 = car->x290.x + D_80095624 * s;
    cam->x40 = car->x290.z + D_80095624 * c;
    cam->x3C = car->x290.y + D_800B8B60;
    Camera_UpdateBasis(cam);
}
