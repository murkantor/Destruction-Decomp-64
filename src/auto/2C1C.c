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

u32 Res_GetSize(s32);
void *Mem_AllocAligned(u32, s32, s32);
void Res_Load(s32, void *);

void *Track_LoadNodeInfo(s32 id) {
    void *p = Mem_AllocAligned(Res_GetSize(id), 7, 0);

    Res_Load(id, p);
    gGame->race->unk34 = p;
    return p;
}
