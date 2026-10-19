#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x10]; s32 x10; u8 pad14[0x9C]; } Controller;
typedef struct { u8 pad[0x10]; s8 x10; } Car;
typedef struct { u8 pad[0x28]; Car *x28; } Player;
extern Vec3f D_800BA378;
extern Vec3f D_800BA384;
extern void *D_8011EEF8[];
Controller *func_80062F14(void);
void func_80063564(s32, s32);
void Sound_PlayAt(void *, Vec3f, Vec3f, s32);
void Cheat_UpdateInput(void);

s32 func_8003F644(Player *p) {
    Vec3f a = D_800BA378;
    Vec3f b = D_800BA384;
    s32 idx = p->x28->x10;
    s32 r;
    Controller *c;

    c = func_80062F14();
    r = 0;
    if (c[idx].x10 & 0x2000) {
        func_80063564(0x20, 1);
        Sound_PlayAt(D_8011EEF8[0], a, b, 7);
        r = 1;
    }
    if (idx == 0) {
        Cheat_UpdateInput();
    }
    return r;
}
