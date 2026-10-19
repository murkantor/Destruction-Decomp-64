#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x24]; } Mtx3;
typedef struct {
    u8 pad0[0xC];
    f32 mass;
    u8 pad10[4];
    f32 x14;
    f32 x18;
    f32 x1C;
    u8 pad20[0xB0 - 0x20];
    Vec3f vel;
    Vec3f angvel;
    Vec3f right;
    Vec3f up;
    Vec3f fwd;
} Body;
typedef struct {
    u8 pad0[0xA8];
    f32 xA8;
    u8 padAC[0xC4 - 0xAC];
    f32 xC4;
    u8 padC8[0x188 - 0xC8];
    f32 x188;
    f32 x18C;
    f32 x190;
    u8 pad194[4];
    f32 x198;
    f32 x19C;
    u8 pad1A0[0xC];
    u8 x1AC;
    u8 pad1AD[0x1BC - 0x1AD];
} Wheel;
typedef struct { u8 x0; u8 x1; u16 x2; } CarCfg;
typedef struct {
    u8 pad0[0x2C];
    CarCfg *cfg;
    u8 pad30[0x1EC - 0x30];
    Body body;
    u8 pad2D8[0x364 - 0x2D8];
    Wheel wheels[4];
    u8 padA54[0xA60 - 0xA54];
    f32 xA60;
    u8 padA64[0xBDC - 0xA64];
    s32 xBDC;
    u8 padBE0[0xBE8 - 0xBE0];
    f32 xBE8;
} Car;
typedef struct { u8 pad[0x10]; s32 x10; } Track;
typedef struct { u8 pad[0x1100]; Track *track; } Game;

extern Game *gGame;
extern f64 D_800B97A8, D_800B97B0, D_800B97B8, D_800B97C0;
extern f64 D_800B97C8, D_800B97D0, D_800B97D8, D_800B97E0, D_800B97E8, D_800B97F0, D_800B97F8, D_800B9800, D_800B9808;
extern f32 sqrtf(f32);
extern void Vec3f_Length(f32 *out, Vec3f *v);
extern void Vec3f_Normalize(Vec3f *out, Vec3f *v);
extern void Vec3f_Dot(f32 *out, Vec3f *a, Vec3f *b);
extern void Vec3f_Cross(Vec3f *out, Vec3f *a, Vec3f *b);
extern void Vec3f_Scale(Vec3f *out, Vec3f *v, f32 s);
extern void Vec3f_Add(Vec3f *out, Vec3f *a, Vec3f *b);
extern void Mtx3_MulVec3fTransposed(Vec3f *out, Vec3f *m, Vec3f *v);
extern void Wheel_ComputeTireForce(Vec3f *out, Car *car, Wheel *w, f32 load, Vec3f *p, s32 *slot, s32 i);

void Car_ApplyWheelForces(Car *car, Vec3f *force, Vec3f *torque, s32 *slots)
{
    Vec3f dir;
    Vec3f grav;
    f32 gLen;
    f32 speed2;
    f32 g;
    f32 k;
    f32 t;
    f32 y;
    f32 s;
    f32 m;
    s32 i;
    u8 flag;
    Body *body = &car->body;
    Vec3f *bvel = &car->body.vel;
    Vec3f *bang = &car->body.angvel;
    Vec3f *bright = &car->body.right;
    Vec3f *bup = &car->body.up;
    Vec3f *bfwd = &car->body.fwd;
    Wheel *w;

    g = body->mass * 12.0;
    if (gGame->track->x10 == 0) {
        k = g * 1.25;
    } else {
        k = g;
    }
    force->y -= k;
    t = k * bup->y;
    grav.x = -t * bup->x;
    grav.y = k - t * bup->y;
    grav.z = -t * bup->z;
    Vec3f_Length(&gLen, &grav);
    Vec3f_Normalize(&dir, bvel);
    Vec3f_Dot(&speed2, bvel, bvel);
    for (i = 0; i < 4; i++) {
        Vec3f F;
        Vec3f tq;
        Vec3f p;
        Vec3f vel;
        f32 vel2;
        f32 fLen;

        w = &car->wheels[i];
        if (w->x19C < 1.0f && 0.0f < w->x198) {
        flag = 1;
        p.x = w->x188;
        p.z = w->x190;
        p.y = y = w->xA8 + -0.14;
        if (y > 0.0) {
            p.y = 0.0f;
        } else if (y < D_800B97A8) {
            p.y = -0.45f;
        }
        if (car->xBDC) {
            p.y = -0.1f;
        }
        Mtx3_MulVec3fTransposed(&p, &body->right, &p);
        Vec3f_Cross(&vel, bang, &p);
        vel.x += bvel->x;
        vel.y += bvel->y;
        vel.z += bvel->z;
        Wheel_ComputeTireForce(&F, car, w, g * 0.25, &p, &slots[i], i);
        Vec3f_Dot(&vel2, &vel, &vel);
        if (w->xC4 == 0.0f && vel2 == 0.0f) {
            Vec3f_Length(&fLen, &F);
            if (fLen > gLen * 0.25) {
                force->x = force->x - grav.x * 0.25;
                force->y = force->y - grav.y * 0.25;
                force->z = force->z - grav.z * 0.25;
                flag = 0;
            }
        }
        if (flag) {
            force->x = force->x + F.x;
            force->y = force->y + F.y;
            force->z = force->z + F.z;
            Vec3f_Cross(&tq, &p, &F);
            if (car->cfg->x2 == 0 && bup->y > D_800B97B0 && car->xBE8 < D_800B97B8 && speed2 > D_800B97C0) {
                Vec3f tmp;
                f32 dot;

                if (gGame->track->x10 == 0) {
                    Vec3f_Dot(&dot, bright, &dir);
                    if (dot < D_800B97C8 && dot > D_800B97D0) {
                        Vec3f_Dot(&dot, bfwd, &dir);
                        if (dot > 0.0) {
                            m = (tq.y > 0.0) ? 0.4f : 2.5f;
                        } else {
                            m = (tq.y > 0.0) ? 2.5f : 0.4f;
                        }
                        tq.y = tq.y * m;
                    }
                }
                Vec3f_Dot(&dot, bfwd, &tq);
                Vec3f_Scale(&tmp, bfwd, -dot);
                Vec3f_Add(&tq, &tq, &tmp);
                Vec3f_Dot(&dot, bright, &tq);
                Vec3f_Scale(&tmp, bright, dot * D_800B97D8);
                Vec3f_Add(&tq, &tq, &tmp);
            }
            {
                Vec3f tmp2;
                f32 dot2;

                Vec3f_Dot(&dot2, bfwd, &tq);
                Vec3f_Scale(&tmp2, bfwd, dot2 * D_800B97E0);
                Vec3f_Add(&tq, &tq, &tmp2);
                {
                    Vec3f tmp3;
                    f32 dot3;

                    Vec3f_Dot(&dot3, bright, &tq);
                    Vec3f_Scale(&tmp3, bright, dot3 * D_800B97E8);
                    Vec3f_Add(&tq, &tq, &tmp3);
                }
            }
            if (((u8 *)car->cfg)[2] != 0 & i < 2) {
                if (speed2 < D_800B97F0) {
                    tq.y *= (f32)((D_800B97F0 - speed2) * D_800B97F8 + D_800B9800);
                }
            }
            torque->x = torque->x + tq.x;
            torque->y = torque->y + tq.y;
            torque->z = torque->z + tq.z;
        }
        } else {
            w->x1AC = 0;
            *(s32 *)&car->xBE8 = 0;
        }
    }
    if (car->xA60 > 0.0f) {
        s = sqrtf(body->x14 * body->x14 + body->x18 * body->x18 + body->x1C * body->x1C) * D_800B9808;
        torque->x = torque->x - s * bang->x;
        torque->y = torque->y - s * bang->y;
        torque->z = torque->z - s * bang->z;
    }
}
