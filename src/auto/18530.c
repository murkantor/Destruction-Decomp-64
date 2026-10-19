#include "common.h"
#include "m2c.h"

typedef struct { u8 a[8]; u8 b[8]; } BufPair;
typedef struct { u8 pad[0x4C]; BufPair x4C[1]; } Model;
extern s8 D_800D31CC;
extern u8 gMainHeap[];
void Heap_Free(void *, void *);
void func_800109E0(Model *);

void func_80017930(Model *o) {
    s32 i;

    for (i = 0; i < D_800D31CC; i++) {
        Heap_Free(gMainHeap, o->x4C[i].a);
        Heap_Free(gMainHeap, o->x4C[i].b);
    }
    func_800109E0(o);
}
