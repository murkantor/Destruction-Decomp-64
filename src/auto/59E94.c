#include "common.h"
#include "m2c.h"

typedef struct {
    s32 unk0;
    s32 channel;
    u8 pad8[0x18];
    s32 unk20;
    u8 pad24[4];
    u64 time28;
    u64 time30;
    s32 unk38;
    s32 unk3C;
    s8 unk40;
    u8 pad41[3];
    s32 status;
    u8 pfs[0x68];
} Rumble;
extern Rumble gControllers[];
extern OSMesgQueue gSiMesgQueue;
u64 osGetTime(void);
s32 osMotorInit(OSMesgQueue *, void *, s32);
s32 __osMotorAccess(void *, s32);
void Pad_Clear(void);

void Rumble_Restart(s32 channel) {
    Rumble *r = &gControllers[channel];

    r->status = osMotorInit(&gSiMesgQueue, r->pfs, channel);
    if (r->status == 0) {
        r->status = __osMotorAccess(r->pfs, 1);
    }
    r->time28 = osGetTime();
}
