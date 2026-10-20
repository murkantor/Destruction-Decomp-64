#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[4];
    u32 offset;
    u8 pad8[0x10];
    u32 size;
} ResEntry;
extern u8 D_00000000[];
extern u8 gResArchiveRom[];
ResEntry *Res_Find(s32);
void Dma_LoadAndFlush(void *, u32, u32);

void Res_Load(s32 id, void *dst) {
    ResEntry *e = Res_Find(id);

#ifdef TARGET_PC
    if (e == NULL) {
        extern void Port_Log(const char *, ...);
        Port_Log("Res_Load: no resource %d (0x%X), from %p", id, id, __builtin_return_address(0));
        return;
    }
#endif
    Dma_LoadAndFlush(dst, e->offset - ((u32)D_00000000 - (u32)gResArchiveRom), e->size);
}
