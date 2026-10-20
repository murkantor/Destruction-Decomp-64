#include "common.h"
#include "m2c.h"

typedef struct {
    u8 data[0x20];
} Entry;
void *Mem_AllocAlignedAlt(u32, u32, u32);
void Mem_Free(void *);
s32 Obj_CreateFromSpawn(Entry *);
void func_80064100(s32);
void Res_Load(s32, void *);
u32 Res_GetSize(s32);

s32 Track_LoadObjects(s32 id) {
    u32 size = Res_GetSize(id);
    Entry *e = Mem_AllocAlignedAlt(size, 0x3F, 0);
    u32 n;
    s32 i;

    Res_Load(id, e);
    n = size >> 5;
    for (i = 0; i < (s32)n; i++) {
        func_80064100(Obj_CreateFromSpawn(&e[i]));
    }
    Mem_Free(e);
    return 0;
}
