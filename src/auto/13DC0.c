#include "common.h"
#include "m2c.h"

typedef struct {
    u8 data[0x20];
} Entry;
void *Mem_AllocAlignedAlt(u32, u32, u32);
void Mem_Free(void *);
s32 func_8001325C(Entry *);
void func_80064100(s32);
void func_80063BFC(s32, void *);
u32 func_80063D6C(s32);

s32 func_800131C0(s32 id) {
    u32 size = func_80063D6C(id);
    Entry *e = Mem_AllocAlignedAlt(size, 0x3F, 0);
    u32 n;
    s32 i;

    func_80063BFC(id, e);
    n = size >> 5;
    for (i = 0; i < (s32)n; i++) {
        func_80064100(func_8001325C(&e[i]));
    }
    Mem_Free(e);
    return 0;
}
