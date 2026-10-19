#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;

extern s32 D_80124364;
extern s32 D_8013137C;
extern s32 D_80125C18;
extern u8 D_80124368[];
extern s32 D_80126068[];
extern s32 D_80125F58[];
extern u64 gGameTime;
extern u64 D_801244D8;
extern u64 D_80131EE0[];
extern u64 D_800D12E0[];
extern void *D_8011EEF8[];
extern u64 Time_Get(u64 *);
extern int rand(void);
extern void func_80007744(void *obj, Vec3f pos, s32 id);

void func_80008818(void) {
    s32 i;

    if (D_80124364 <= 0) {
        if (D_8013137C == -1) {
            return;
        }
    }
    if (D_80124364 <= 0 && D_8013137C >= 0) {
        D_80126068[0] = D_8013137C;
        D_80125F58[0] = D_80125C18;
        D_80124368[D_8013137C] = 1;
        D_80124364++;
    }
    D_8013137C = -1;
    if (D_80125F58[0] > 0) {
        if (Time_Get(&gGameTime) < D_801244D8) {
            D_801244D8 = D_801244D8 + D_80125F58[0] * 1000000.0;
        } else {
            D_801244D8 = Time_Get(&gGameTime) + D_80125F58[0] * 1000000.0;
        }
        D_80125F58[0] = 0;
    }
    if (Time_Get(&gGameTime) < D_801244D8) {
        return;
    }
    D_801244D8 = D_800D12E0[D_80126068[0]] = Time_Get(&gGameTime) + D_80131EE0[D_80126068[0]];
    D_80124368[D_80126068[0]] = 1;
    {
        Vec3f p;
        s32 id;
        p.x = (rand() % 56) + 100.0;
        p.y = 0;
        p.z = 0;
        id = D_80126068[0];
        func_80007744(D_8011EEF8[0], p, id);
    }
    for (i = 0; i < D_80124364; i++) {
        D_80126068[i] = D_80126068[i + 1];
        D_80125F58[i] = D_80125F58[i + 1];
    }
    D_80126068[D_80124364] = -1;
    D_80125F58[D_80124364] = 0;
    D_80124364--;
}
