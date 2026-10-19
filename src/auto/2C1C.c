#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x10];
    s32 unk10;
    u8 pad14[0x20];
    void *unk34;
} Race;
typedef struct {
    u8 pad[0xC];
    s16 mode;
    u8 padE[0x104A];
    u64 time;
    u8 pad1060[0x9C];
    s16 unk10FC;
    u8 pad10FE[0x2];
    Race *race;
} Game;
extern Game *gGame;

u32 func_80063D6C(s32);
void *Mem_AllocAligned(u32, s32, s32);
void func_80063BFC(s32, void *);

void *func_8000201C(s32 id) {
    void *p = Mem_AllocAligned(func_80063D6C(id), 7, 0);

    func_80063BFC(id, p);
    gGame->race->unk34 = p;
    return p;
}
