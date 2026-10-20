#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;

typedef struct Debris {
    f32 x0;
    Vec3f x4;
    f32 x10;
    f32 x14;
    f32 x18;
    f32 x1C;
    f32 x20;
    f32 x24;
    u8 x28[0x50 - 0x28];
    struct Debris *x50;
} Debris;

typedef struct {
    u8 pad0[0x34];
    s32 x34;
    u8 pad38[0x178 - 0x38];
    u8 x178[0x290 - 0x178];
    Vec3f x290;
} Car;

extern s8 D_800D31CC;
extern u32 D_80125C58;
extern s32 D_801311B8;
extern Debris *D_80125B70[];
void *Mem_Alloc(u32 size);
void ModelInst_Init(void *, void *);
void ModelInst_SetScale(void *, f32);
void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
s32 rand(void);

static inline s32 GetCount(s32 amount) {
    if (amount > 50) {
        return 8;
    } else if (amount > 40) {
        return 7;
    } else if (amount > 20) {
        return 6;
    } else if (amount > 10) {
        return 5;
    } else if (amount > 5) {
        return 4;
    } else {
        return 2;
    }
}

void Debris_Spawn(Car *car, Vec3f pos, s32 amount) {
    s8 idx;
    s8 n;
    s8 i;
    Debris *p;
    void *q;

    if (D_800D31CC >= 3) {
        return;
    }
    idx = car->x34;
    pos.y += 1.0;
    n = GetCount(amount);
    for (i = 0; i < n; i++) {
        if (D_80125C58 <= 0x2000) {
            return;
        }
        if (D_801311B8 >= 0x4C) {
            return;
        }
        p = Mem_Alloc(0x54);
        if (p == NULL) {
            return;
        }
        q = p->x28;
        D_801311B8++;
        p->x50 = D_80125B70[(s8)idx];
        D_80125B70[(s8)idx] = p;
        ModelInst_Init(q, car->x178);
        ModelInst_SetScale(q, rand() * (1.0f / 32767) * 0.0006 + 0.001);
        Vec3f_Add(&p->x4, &car->x290, &pos);
        p->x0 = p->x4.y;
        p->x10 = rand() * (1.0f / 32767) * 0.02 + 0.02;
        p->x14 = rand() * (1.0f / 32767) * 0.02 + 0.02;
        p->x18 = rand() * (1.0f / 32767) * 0.02 + 0.02;
        p->x1C = rand() * (1.0f / 32767) * 7.0 - 3.5;
        p->x20 = rand() * (1.0f / 32767) * 3.0 + 2.5;
        p->x24 = rand() * (1.0f / 32767) * 7.0 - 3.5;
    }
}
