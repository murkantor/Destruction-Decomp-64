#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x14]; s32 x14; u8 pad2[0x30]; } Menu48;
typedef struct { s32 a; s32 b; } Pair;
typedef struct { Pair p[3]; } Pair3;
typedef struct { s32 n; u8 pad[0x18]; void *x1C[4]; } Players;
extern Menu48 D_800BA704;
extern Pair3 D_800BA74C;
extern Players D_800958A4;
extern void *gUi[];
void func_8003EA7C(s32, s32);
void func_8005F050(void *, Menu48 *);
void func_8005D8B8(void *, s32, s32);
void func_80041E44(void *, s32, s32);

void func_800430E8(void) {
    Menu48 m = D_800BA704;
    Pair3 t = D_800BA74C;
    s32 i;

    func_8003EA7C(t.p[D_800958A4.n - 2].a, t.p[D_800958A4.n - 2].b);
    m.x14 = D_800958A4.n + 0x11506;
    func_8005F050(gUi[0], &m);
    func_8005D8B8(gUi[0], 2, 0x3B);
    for (i = 0; i < D_800958A4.n; i++) {
        func_80041E44(D_800958A4.x1C[i], -1, i);
    }
}
