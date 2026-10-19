#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;

typedef struct {
    u8 pad[0x1EC];
    u8 x1EC[0x290 - 0x1EC];
    f32 x290;
    f32 x294;
    f32 x298;
    u8 pad2[0xBAC - 0x29C];
    u8 xBAC;
    u8 pad3[0xBC0 - 0xBAD];
    f32 xBC0;
    s32 xBC4;
    u8 pad4[4];
    f32 xBCC;
    s32 xBD0;
} Car;

typedef struct {
    u8 pad[0x74];
    Car *cars[12];
    u8 pad2[0xA4 - 0xA4];
    s32 xA4;
} Race;

typedef struct {
    u8 pad[0x1100];
    Race *x1100;
} Game;

extern Game *gGame;
s32 rand(void);
void Math_SinCos(f32, f32 *, f32 *);
void func_80028070(void *, Vec3f, void *, Vec3f, f32);

void func_8002E740(void) {
    s32 list[32];
    Vec3f dirs[2];
    s32 n;
    s32 i, j, k;
    s32 *p;
    Car *a, *b;
    f32 *posA;
    s32 maskA, maskB;
    f32 d;
    f32 r1, r2;

    n = 0;
    for (i = 0; i < gGame->x1100->xA4; i++) {
        if (gGame->x1100->cars[i]->xBAC) {
            list[n++] = i;
        }
    }
    for (i = 0; i < n - 1; i++) {
        a = gGame->x1100->cars[list[i]];
        posA = &a->x290;
        maskA = 1 << list[i];
        for (j = i + 1; j < n; j++) {
            b = gGame->x1100->cars[list[j]];
            d = (b->x290 - posA[0]) * (b->x290 - posA[0]) + (b->x294 - posA[1]) * (b->x294 - posA[1]) +
                (b->x298 - posA[2]) * (b->x298 - posA[2]);
            maskB = 1 << list[j];
            if ((a->xBD0 & maskB) && (b->xBD0 & maskA)) {
                if (d > 16.0) {
                    a->xBD0 &= ~maskB;
                    b->xBD0 &= ~maskA;
                }
            } else if (d < 16.0) {
                a->xBD0 |= maskB;
                b->xBD0 |= maskA;
                a->xBC4 = 0;
                b->xBC4 = 0;
                r1 = rand() * (1.0f / 32767.0f);
                r2 = rand() * (1.0f / 32767.0f);
                a->xBCC = r1 * 0.1 + 0.05;
                b->xBCC = r2 * 0.1 + 0.05;
                if (r1 > 0.5) {
                    a->xBC0 = 0.25f;
                    b->xBC0 = 0.75f;
                } else {
                    a->xBC0 = 0.75f;
                    b->xBC0 = 0.25f;
                }
                for (k = 0; k < 2; k++) {
                    Math_SinCos(rand() * (1.0f / 32767.0f) * 6.2831853, &dirs[k].x, &dirs[k].z);
                    dirs[k].y = 0.0f;
                }
                func_80028070(a->x1EC, dirs[0], b->x1EC, dirs[1], 5.0f);
            }
        }
    }
}
