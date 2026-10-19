#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct {
    u8 pad[0x50];
    Vec3f x50;
    Vec3f x5C;
} Basis;
typedef struct {
    u8 pad[0x1EC];
    Basis basis;
} Target;
typedef struct { u8 pad[0x20]; Target *x20; } CamLink;
typedef struct {
    u8 pad[0x38];
    Vec3f x38;
    u8 pad44[0x54 - 0x44];
    Vec3f x54;
    u8 pad60[0x68 - 0x60];
    Vec3f x68;
    u8 pad74[0xB0 - 0x74];
    CamLink *xB0;
    u8 xB4;
    u8 xB5;
    u8 xB6;
} Camera;
extern f32 D_800955D8, D_800955DC, D_800955E0, D_800955E4, D_800955E8, D_800955EC, D_800955F0, D_800955F4, D_800955F8;
Vec3f func_8001FBA8(Target *);
void Vec3f_Scale(Vec3f *, Vec3f *, f32);
void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);

void Camera_UpdateChase(Camera *cam) {
    Target *t = cam->xB0->x20;
    Basis *b;
    Vec3f eye;
    Vec3f tmp;
    Vec3f at;
    Vec3f pos;
    f32 back, up, side;

    pos = func_8001FBA8(t);
    b = &t->basis;
    switch (cam->xB4) {
    case 0:
        cam->x68 = b->x5C;
        back = D_800955D8;
        up = D_800955DC;
        side = D_800955E0;
        break;
    case 1:
        back = D_800955E4;
        up = D_800955E8;
        side = D_800955EC;
        cam->x68.x = 0.0f;
        cam->x68.z = 0.0f;
        cam->x68.y = 1.0f;
        break;
    case 2:
        back = D_800955F0;
        up = D_800955F4;
        side = D_800955F8;
        cam->x68.x = 0.0f;
        cam->x68.z = 0.0f;
        cam->x68.y = 1.0f;
        break;
    case 6:
        back = D_800955F4;
        up = D_800955F0;
        side = D_800955F8;
        cam->x68.x = 0.0f;
        cam->x68.z = 0.0f;
        cam->x68.y = 1.0f;
        break;
    default:
        return;
    }
    Vec3f_Scale(&eye, &b->x50, back);
    Vec3f_Scale(&tmp, &b->x5C, side);
    Vec3f_Add(&eye, &eye, &tmp);
    Vec3f_Add(&eye, &eye, &pos);
    at.x = b->x5C.x * up + pos.x;
    at.y = pos.y;
    at.z = b->x5C.z * up + pos.z;
    cam->x38 = eye;
    cam->x54 = at;
    cam->xB6 = 0;
}
