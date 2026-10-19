#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Menu;
extern s32 D_800958A8;
extern s32 D_800958B0;
extern s32 D_800958B4;
extern s32 D_800958C0;
extern s32 *D_80095CA8[];
void func_8002D0E0(s32);
void func_80063564(s32, s32);

s32 func_8003FB78(Menu *m) {
    D_800958B0 = m->x10;
    if (D_800958A8 == 0) {
        D_800958B4 = *D_80095CA8[m->x10];
        func_8002D0E0(D_800958B0);
        func_80063564(0x18, 1);
        m->x10 = D_800958C0;
    } else {
        func_8002D0E0(D_800958B0);
        func_80063564(0x17, 1);
    }
    return 1;
}
