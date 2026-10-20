#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct Contact {
    u8 pad0[0xC];
    s32 xC;
    Vec3f n;
    u8 pad1C[0x2C - 0x1C];
    Vec3f pos;
    u8 pad38[0x5C - 0x38];
    struct Contact *next;
} Contact;
typedef struct {
    u8 pad0[0xB0];
    Vec3f vel;
    Vec3f angvel;
    u8 padC8[0x10];
    f32 xD8;
    u8 padDC[0x164 - 0xDC];
    Contact *contacts;
    s32 x168;
    s32 x16C;
} Car;
typedef struct { u8 pad[0xBE8]; s32 xBE8; } Player;
typedef struct { u8 pad[0x10]; s32 x10; } Track;
typedef struct { u8 pad[0x1100]; Track *track; } Game;

typedef struct { Vec3f v[2]; } Vec3f2;
extern Vec3f2 D_800B9570;
extern Game *gGame;
extern f32 sqrtf(f32);
extern void Vec3f_Dot(f32 *out, Vec3f *a, Vec3f *b);
extern void Vec3f_Length(f32 *out, Vec3f *v);
extern void Vec3f_Add(Vec3f *out, Vec3f *a, Vec3f *b);
extern void Vec3f_Scale(Vec3f *out, Vec3f *v, f32 s);
#ifdef TARGET_PC
#include "port_sret.h"
#else
extern void Contact_ComputeImpulse(Vec3f *out, Player *p, Contact *c);
#endif
extern u8 Body_EnergyRatioExceeds(Car *car, Vec3f *imp);
extern void Car_OnCollision2(Car *car, Vec3f pos, Car *car2, Vec3f pos2, f32 len);

void Car_ApplyContactImpulses(Player *p, Car *car)
{
    Vec3f2 acc2 = D_800B9570;
    f32 speed2;
    s32 n = 0;
    s32 flag;
    Contact *c;
    f32 scale;
    f32 *s7 = (f32 *)((u8 *)car + 0xC8);
    f32 mag2;
    f32 s;
    f32 bx;

    if (car->x168) {
        flag = 0;
        c = car->contacts;
        Vec3f_Dot(&speed2, &car->vel, &car->vel);
        do {
            Vec3f imp[2];
            f32 len;

            if (gGame->track->x10 == 0 && c->n.y > 0.2 && c->n.y < 0.707 &&
                car->vel.x * c->n.x + car->vel.z * c->n.z < 0.0f) {
                flag = 1;
            }
            Contact_ComputeImpulse(imp, p, c);
            if (!Body_EnergyRatioExceeds(car, imp)) {
                Vec3f_Add(&acc2.v[0], &acc2.v[0], &imp[0]);
                Vec3f_Add(&acc2.v[1], &acc2.v[1], &imp[1]);
                Vec3f_Length(&len, &imp[0]);
                car->x16C = c->xC;
                n++;
                Car_OnCollision2(car, c->pos, car, c->pos, len);
            }
            c = c->next;
        } while (c);
        if (n) {
            scale = 1.0 / n;
            if (speed2 > 576.0) {
                Vec3f_Scale(&acc2.v[0], &acc2.v[0], scale * 0.6);
            } else {
                Vec3f_Scale(&acc2.v[0], &acc2.v[0], scale * 0.8);
            }
            if (gGame->track->x10 == 0) {
                if (s7[4] < 0.6) {
                    Vec3f_Scale(&acc2.v[1], &acc2.v[1], scale * 0.7);
                } else if (speed2 > 576.0) {
                    Vec3f_Scale(&acc2.v[1], &acc2.v[1], scale * 0.36);
                } else {
                    Vec3f_Scale(&acc2.v[1], &acc2.v[1], scale * 0.5);
                }
            } else if (speed2 > 576.0 && !(s7[4] < 0.6)) {
                Vec3f_Scale(&acc2.v[1], &acc2.v[1], scale * 0.35);
            } else {
                Vec3f_Scale(&acc2.v[1], &acc2.v[1], scale * 0.7);
            }
            Vec3f_Add(&car->vel, &car->vel, &acc2.v[0]);
            Vec3f_Add(&car->angvel, &car->angvel, &acc2.v[1]);
            Vec3f_Scale(&car->vel, &car->vel, 0.99f);
            if (flag && car->vel.y > 0.0) {
                car->vel.y = 0.0f;
            }
            p->xBE8 = 0;
            bx = car->angvel.x;
            mag2 = bx * bx + car->angvel.y * car->angvel.y + car->angvel.z * car->angvel.z;
            if (9.0f < mag2) {
                mag2 = 3.0f / sqrtf(mag2);
                car->angvel.x = bx * mag2;
                car->angvel.y = car->angvel.y * mag2;
                car->angvel.z = car->angvel.z * mag2;
            }
        }
    }
}
