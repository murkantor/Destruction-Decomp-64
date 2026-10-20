#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { f32 x, y, z; } Vec3;
typedef struct { f32 m[3][3]; } Mtx3;
typedef struct { f32 m[4][4]; } Mtx4;

typedef struct {
    Vec3 pos;
    Vec3 rot;
} SubA;

typedef struct {
    f32 x0;
    Vec3 pos;
    Mtx3 rot;
} SubB;

typedef struct {
    u8 pad0[8];
    Vec3 pos;
    Mtx3 rot;
} SubC;

typedef struct {
    u8 pad0[8];
    void *frame;
    u8 padC[4];
    Vec3 pos;
    Mtx3 rot;
    u8 pad40[8];
    u8 x48;
} SubD;

typedef struct {
    void *frame;
} SubE;

typedef struct {
    s16 type;
    u8 pad2[0xE];
    u8 model[0x28];
    void *sub;
} Obj;

typedef struct { s32 x0; } Info;
typedef struct {
    u8 pad0[0x1074];
    u8 x1074;
    u8 pad1075[0x1100 - 0x1075];
    Info *x1100;
} Game;

extern Game *gGame;
extern void func_8005AB1C(void *, f32, f32, f32);
extern void func_8005AB34(void *, f32, f32, f32);
extern void func_8005AB4C(void *, f32);
extern void func_8005AB5C(void *, Mtx4 *);
extern void Frame_ToScaledMatrix(Mtx4 *, void *);
extern void Mtx3_RotateY(Mtx3 *, f32);
extern void Mtx_FromRotPosScaled(void *, Vec3, Mtx3 *);
extern Gfx *Model_Draw(void *, Gfx *);

Gfx *Obj_DrawModel(Obj *o, Gfx *gfx) {
    switch (o->type) {
        case 23:
            func_8005AB4C(o->model, 0.01f);
            goto case_a;
        case 30: {
            SubA *s = o->sub;
            func_8005AB1C(o->model, s->pos.x, s->pos.y, s->pos.z);
            func_8005AB34(o->model, s->rot.x * 0.017453292500000002, s->rot.y * 0.017453292500000002, s->rot.z * 0.017453292500000002);
            break;
        }
        case 29:
            func_8005AB4C(o->model, 0.15f);
        case 27:
        case 28:
        case 32:
        case 33: {
            SubA *s;
        case_a:
            s = o->sub;
            func_8005AB1C(o->model, s->pos.x, s->pos.y, s->pos.z);
            func_8005AB34(o->model, s->rot.x, s->rot.y, s->rot.z);
            break;
        }
        case 19: {
            SubB *s = o->sub;
            Mtx_FromRotPosScaled(o->model, s->pos, &s->rot);
            break;
        }
        case 6: {
            SubC *s = o->sub;
            Mtx_FromRotPosScaled(o->model, s->pos, &s->rot);
            break;
        }
        case 14: {
            Mtx4 m;
            void *f = ((SubE *)o->sub)->frame;
            Info *inf = gGame->x1100;
            if (f == NULL) {
                return gfx;
            }
            if (inf->x0 < 2) {
                return gfx;
            }
            Frame_ToScaledMatrix(&m, f);
            func_8005AB5C(o->model, &m);
            break;
        }
        case 15:
        case 17: {
            SubD *s = o->sub;
            if (s->frame != NULL) {
                Mtx4 m;
                Frame_ToScaledMatrix(&m, s->frame);
                m.m[0][0] = m.m[0][0] * 2.0;
                m.m[0][1] = m.m[0][1] * 2.0;
                m.m[0][2] = m.m[0][2] * 2.0;
                m.m[1][0] = m.m[1][0] * 2.0;
                m.m[1][1] = m.m[1][1] * 2.0;
                m.m[1][2] = m.m[1][2] * 2.0;
                m.m[2][0] = m.m[2][0] * 2.0;
                m.m[2][1] = m.m[2][1] * 2.0;
                m.m[2][2] = m.m[2][2] * 2.0;
                func_8005AB5C(o->model, &m);
            } else {
                Vec3 p;
                p = s->pos;
                p.y = p.y - 2.8;
                s->x48 = (s8)(s->x48 + 1) % 120;
                if ((s8)s->x48 < 60) {
                    Mtx3_RotateY(&s->rot, -0.01f);
                } else {
                    Mtx3_RotateY(&s->rot, 0.01f);
                }
                Mtx_FromRotPosScaled(o->model, p, &s->rot);
            }
            break;
        }
        case 16:
        case 18: {
            SubC *s = o->sub;
            if (gGame->x1074 == 0) {
                Mtx3_RotateY(&s->rot, 0.03f);
            }
            Mtx_FromRotPosScaled(o->model, s->pos, &s->rot);
            break;
        }
    }
    return Model_Draw(o->model, gfx);
}
