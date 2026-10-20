#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef f32 Mtx4[4][4];
typedef struct { u8 pad0[0xA4]; Vec3f xA4; u8 padB0[0x18]; Vec3f xC8[3]; u8 padEC[0x30]; f32 x11C; u8 pad120[0x58]; u8 x178[0x1BC]; u8 x334[0x1BC]; u8 x4F0[0x1BC]; u8 x6AC[0x1BC]; } Sub;
typedef struct { u8 pad0[0x98]; u8 x98[0x28]; u8 xC0[0x28]; u8 xE8[0x28]; u8 x110[0x28]; u8 x138[0xB4]; Sub x1EC; } Car;
typedef struct { u8 pad0[0xC]; s16 xC; } Game;
extern Game *gGame;
extern u8 D_800958A1;
void Mtx_FromBodyMul(Mtx4 *, Mtx4 *, void *);
void func_8005AB5C(void *, Mtx4 *);

void Car_UpdateModelMatrices(Car *car) {
    Mtx4 m[5];
    Mtx4 unused;
    Sub *s = &car->x1EC;
    Sub *w = s;
    s32 i;
    f32 f;

    for (i = 0; i < 3; i++) {
        m[0][i][0] = s->xC8[i].x;
        m[0][i][1] = s->xC8[i].y;
        m[0][i][2] = s->xC8[i].z;
        m[0][i][3] = 0.0f;
    }
    m[0][3][0] = s->xA4.x;
    m[0][3][1] = s->xA4.y;
    m[0][3][2] = s->xA4.z;
    m[0][3][3] = f = 1.0f;
    Mtx_FromBodyMul(&m[1], &m[0], w->x178);
    Mtx_FromBodyMul(&m[2], &m[0], w->x334);
    Mtx_FromBodyMul(&m[3], &m[0], w->x4F0);
    Mtx_FromBodyMul(&m[4], &m[0], w->x6AC);
    if (D_800958A1 && gGame->xC == 2) {
        f = -1.0f;
    }
    m[0][0][0] *= s->x11C;
    m[0][0][1] *= s->x11C;
    m[0][0][2] *= s->x11C;
    m[0][1][0] *= s->x11C;
    m[0][1][1] *= s->x11C;
    m[0][1][2] *= s->x11C;
    m[0][2][0] *= s->x11C * f;
    m[0][2][1] *= s->x11C * f;
    m[0][2][2] *= s->x11C * f;
    func_8005AB5C(car->x98, &m[0]);
    func_8005AB5C(car->xC0, &m[1]);
    func_8005AB5C(car->xE8, &m[2]);
    func_8005AB5C(car->x110, &m[3]);
    func_8005AB5C(car->x138, &m[4]);
}
