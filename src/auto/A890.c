#include "common.h"
#include "m2c.h"

typedef struct { void *x0; u8 pad[4]; } Ent8;
typedef struct { u8 x0; u8 x1; u8 x2; u8 x3; struct Mat30 *x4; } Map8;
typedef struct Mat30 { u8 pad[4]; void *x4; u8 pad2[0x28]; } Mat30;
typedef struct { u8 pad[0x18]; Ent8 *x18; Map8 *x1C; } Res;
typedef struct { u8 pad[0x30]; s32 x30; u8 pad2[0x15C]; Res *x190; u8 pad3[0x99C]; u8 xB30[1]; } Obj;
void Car_SpawnSpark(Obj *, s32);
void Commentary_PickRandom(s32 *, s32, s32, s32);
s32 rand(void);

void func_80009C90(Obj *obj, u32 idx) {
    Res *r;
    Map8 *m;
    Mat30 *mat;
    void *tex;
    s32 buf[2];

    if (obj->xB30[idx] == 0) {
        obj->xB30[idx] = 1;
        r = obj->x190;
        if (r->x1C != NULL) {
            m = (Map8 *)(idx * 8 + (u32)r->x1C);
            tex = r->x18[m->x1].x0;
            mat = m->x4;
            if (mat != NULL) {
                if (idx < 2) {
                    mat->x4 = r->x18[m->x2].x0;
                    mat++;
                }
                mat->x4 = tex;
                if (idx == 0) {
                    Car_SpawnSpark(obj, 0);
                    if (obj->x30 == 0 && rand() % 100 < 36) {
                        buf[0] = 0xB1;
                        buf[1] = 0x9F;
                        Commentary_PickRandom(buf, 2, 0, 1);
                    }
                } else if (idx == 1) {
                    Car_SpawnSpark(obj, 1);
                }
            }
        }
    }
}
