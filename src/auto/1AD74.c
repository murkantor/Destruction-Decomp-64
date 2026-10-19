#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x34]; u8 x34[8]; u8 x3C[8]; } Model;
extern u8 D_8008F820;
extern s8 D_800D31CC;
extern Model D_800D1AA0;
extern u8 gMainHeap[];
void Heap_Free(void *, void *);
void func_800109E0(Model *);

void func_8001A174(void) {
    s8 i;
    Model *m;

    if (D_8008F820) {
        return;
    }
    for (i = 0; i < D_800D31CC; i++) {
        m = &D_800D1AA0;
        Heap_Free(gMainHeap, m->x34);
        Heap_Free(gMainHeap, m->x3C);
        func_800109E0(m);
    }
    D_8008F820 = 1;
}
