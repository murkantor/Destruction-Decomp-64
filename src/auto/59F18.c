#include "common.h"
#include "m2c.h"

typedef struct {
    s32 type;
    s32 channel;
    u32 prevButtons;
    u32 buttons;
    u32 pressed;
    s16 prevStickX;
    s16 prevStickY;
    s16 stickX;
    s16 stickY;
    s16 deltaX;
    s16 deltaY;
    s32 rumbleFrames;
    u8 pad24[4];
    u64 rumbleCheckTime;
    u64 idleCheckTime;
    u32 rumbleCount;
    u32 rumblePeriod;
    u8 motorOn;
    u8 pad41[3];
    s32 status;
    u8 pfs[0x68];
} Controller;
typedef struct {
    u16 button;
    s8 stick_x;
    s8 stick_y;
    u8 errnum;
} OSContPad;
extern Controller gControllers[];
extern OSContPad gContPads[];
extern u8 gContPattern;
extern s32 gRumbleRetry;
extern OSMesgQueue gSiMesgQueue;
u64 osGetTime(void);
s32 osMotorInit(OSMesgQueue *, void *, s32);
s32 __osMotorAccess(void *, s32);
void Rumble_Restart(s32);
void Rumble_Stop(s32);
void Rumble_AllowRetry(void);
void func_80058B78(void);

void Rumble_Stop(s32 channel) {
    Controller *c = &gControllers[channel];
    s32 i;

    if (gRumbleRetry) {
        c->status = osMotorInit(&gSiMesgQueue, c->pfs, channel);
        if (c->status == 0) {
            for (i = 0; i < 3; i++) {
                c->status = __osMotorAccess(c->pfs, 0);
            }
        }
    }
    c->idleCheckTime = osGetTime();
}
