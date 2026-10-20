#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0xBA0]; s32 xBA0; } Car;
typedef struct { u8 pad[0x74]; Car *x74[12]; s32 xA4; } Level;
typedef struct { u8 pad[0x1100]; Level *level; } Game;
typedef struct {
    f32 x0; f32 x4; f32 x8; f32 xC; f32 x10;
    u8 x14[0x24];
    f32 x38; f32 x3C; f32 x40;
    Vec3f x44;
} Particle;
typedef struct {
    s16 type;
    u8 pad2[2];
    f32 x4; f32 x8; f32 xC;
    u8 x10[0x28];
    Particle *x38;
    u8 pad3C[4];
    f32 x40; f32 x44; f32 x48;
    Vec3f x4C;
    f32 x58; f32 x5C;
    s32 x60; s32 x64; s32 x68;
} WObj;
typedef struct { u8 pad[0x40C]; WObj *x40C[0x241]; s16 xD10; s16 xD12; } World;
extern Game *gGame[];
extern f64 D_800B9CE0;
extern f64 D_800B9D60;
void Car_UpdatePhysics(WObj *, f32);
void func_80021700(WObj *, f32);
void Vec3f_Scale(Vec3f *, Vec3f *, f32);
void func_8003A4D8(void *, Vec3f *);

void World_UpdateEffects(World *w, f32 dt) {
    s32 i;
    Vec3f t1;
    Vec3f t2;
    f64 g;

    for (i = 0; i < gGame[0]->level->xA4; i++) {
        gGame[0]->level->x74[i]->xBA0 = -1;
    }
    i = 0;
    if (i < w->xD12) {
    g = D_800B9CE0;
    do {
        WObj *o = w->x40C[i];
        switch (o->type) {
        case 3:
            Car_UpdatePhysics(o, dt);
            break;
        case 9:
            func_80021700(o, dt);
            break;
        case 6: {
            Particle *p = o->x38;
            if (p->x0 == -1.0f) {
                break;
            }
            p->x0 += dt;
            p->x8 += p->x38 * dt;
            p->xC += p->x3C * dt;
            p->x10 += p->x40 * dt;
            Vec3f_Scale(&t1, &p->x44, dt);
            func_8003A4D8(p->x14, &t1);
            p->x3C = p->x3C - dt * g;
            break;
        }
        case 4:
        case 5:
        case 7:
        case 8:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
            break;
        case 31: {
            WObj *q = o;
            if (q->x60 == 0) {
                break;
            }
            if (q->x64 == 0) {
                break;
            }
            if (q->x68 == 1) {
                if (q->x58 < q->x5C) {
                    f32 s = dt;
                    if (q->x5C < q->x58 + dt) {
                        s = q->x5C - q->x58;
                    }
                    Vec3f_Scale(&t2, &q->x4C, s);
                    func_8003A4D8(q->x10, &t2);
                }
            } else {
                q->x4 += q->x40 * dt;
                q->x8 += q->x44 * dt;
                q->xC += q->x48 * dt;
                Vec3f_Scale(&t2, &q->x4C, dt);
                func_8003A4D8(q->x10, &t2);
                q->x44 = q->x44 - dt * g;
            }
            q->x58 += dt;
            if (D_800B9D60 <= q->x58) {
                q->x60 = 0;
            }
            break;
        }
        }
    } while (++i < w->xD12);
    }
}
