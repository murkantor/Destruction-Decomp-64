#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x6C];
    s32 x6C;
    u8 pad70[8];
    u8 x78[0x28];
} Thing;
extern Thing *D_800960B0;
extern s32 D_800960B4;
void Mem_Free(void *);
void func_8005AAC0(void *);
void func_80063B54(s32);

void func_8002DCBC(void) {
    s32 i;
    Thing *t;

    for (i = 0; i < D_800960B4; i++) {
        t = &D_800960B0[i];
        func_8005AAC0(t->x78);
        func_80063B54(t->x6C);
    }
    if (D_800960B0 != NULL) {
        Mem_Free(D_800960B0);
        D_800960B0 = NULL;
    }
    D_800960B4 = 0;
}
