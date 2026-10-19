#include "common.h"
#include "m2c.h"

typedef struct { s32 v[4]; } SfxIds;
extern SfxIds D_800BACB0;
void Timer_Start(void *, s64);
void func_800100BC(s32, s32);

void func_80046630(void *m, s32 a, s32 b) {
    SfxIds ids = D_800BACB0;

    Timer_Start(m, 200000000);
    func_800100BC(ids.v[a], 0x111F8);
    func_800100BC(ids.v[b], 0x111F4);
}
