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
extern Game *gGame[];

extern u64 gGameTime;
u64 Time_Get(u64 *);

u64 Game_GetTime(void) {
    if (gGame[0]->mode != 2 || gGame[0]->unk10FC == 0x1F) {
        return Time_Get(&gGameTime);
    }
    return Time_Get(&gGame[0]->time);
}
