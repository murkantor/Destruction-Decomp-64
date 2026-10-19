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

s32 func_80027CF4(u8 *a, u8 *b) {
    s32 sa;
    s32 sb;

    if (gGame->race->unk10 == 1) {
        sa = *(s32 *)(a + 0x18) + *(s32 *)(a + 0x20);
        sb = *(s32 *)(b + 0x18) + *(s32 *)(b + 0x20);
    } else {
        sa = *(s32 *)(a + 0x18) + *(s32 *)(a + 0x1C);
        sb = *(s32 *)(b + 0x18) + *(s32 *)(b + 0x1C);
    }
    return sa >= sb;
}
