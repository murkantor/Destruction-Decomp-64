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
void func_80058848(void *, u32, u32);

void Res_Load(s32 id, void *dst) {
    ResEntry *e = Res_Find(id);

    func_80058848(dst, e->offset - ((u32)D_00000000 - (u32)gResArchiveRom), e->size);
}
