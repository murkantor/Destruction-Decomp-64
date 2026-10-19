#include "common.h"
#include "m2c.h"

typedef struct Debris {
    f32 x0;
    f32 x4, x8, xC;
    f32 x10, x14, x18;
    f32 x1C, x20, x24;
    u8 x28[0x28];
    struct Debris *x50;
} Debris;
typedef struct { u8 pad0[0xC]; s16 xC; u8 padE[0x1058 - 0xE]; u64 x1058; u8 pad1060[0x14]; u8 x1074; } Game;
typedef struct { u8 pad[0x34]; s32 x34; } Obj;
extern Game *volatile gGame;
extern Debris *D_80125B70[];
extern u32 D_80125C58;
extern s32 D_801311B8;
f32 func_80063718(u64 *);
void func_8005AB1C(void *, f32, f32, f32);
void func_8005AB34(void *, f32, f32, f32);
void *Model_Draw(void *, void *);
void func_8005AAC0(void *);
void Mem_Free(void *);

void *Debris_UpdateDraw(Obj *obj, void *gfx) {
    Debris *p;
    Debris *prev;
    Debris *tmp;
    f32 dt;
    s32 id = obj->x34;

    if (gGame->xC != 2) return gfx;
    if (D_80125B70[(s8)id] == NULL) return gfx;
    p = D_80125B70[(s8)id];
    {
        dt = func_80063718(&gGame->x1058);
        prev = NULL;
        if (gGame->x1074) dt = 0.0f;
        do {
            func_8005AB1C(p->x28, p->x4, p->x8, p->xC);
            func_8005AB34(p->x28, p->x10, p->x14, p->x18);
            if (D_80125C58 > 0x2000) {
                gfx = Model_Draw(p->x28, gfx);
            }
            if (p->x8 - p->x0 < -5.0) {
                tmp = p;
                func_8005AAC0(p->x28);
                if (prev) {
                    prev->x50 = p->x50;
                } else {
                    D_80125B70[(s8)id] = p->x50;
                }
                p = p->x50;
                Mem_Free(tmp);
                D_801311B8--;
            } else {
                p->x4 += p->x1C * dt;
                p->x8 += p->x20 * dt;
                p->xC += p->x24 * dt;
                p->x10 += dt * 90.0;
                p->x14 += dt * 90.0;
                p->x18 += dt * 270.0;
                if (p->x10 > 360.0) p->x10 -= 360.0;
                if (p->x14 > 360.0) p->x14 -= 360.0;
                if (p->x18 > 360.0) p->x18 -= 360.0;
                p->x20 -= dt * 12.8;
                prev = p;
                p = p->x50;
            }
        } while (p != NULL);
    }
    return gfx;
}
