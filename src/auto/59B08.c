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
void Pad_Read(void);

void Pad_Update(s32 channel) {
    Controller *c = &gControllers[channel];

    if (c->type == 1) {
        c->prevStickX = c->stickX;
        c->prevStickY = c->stickY;
        c->stickX = gContPads[channel].stick_x;
        c->stickY = gContPads[channel].stick_y;
        c->prevButtons = c->buttons;
        c->deltaX = c->stickX - c->prevStickX;
        c->deltaY = c->stickY - c->prevStickY;
        c->buttons = gContPads[channel].button;
        c->pressed = c->buttons & ~c->prevButtons;
        if (c->rumbleFrames != 0) {
            if (!c->motorOn) {
                c->status = __osMotorAccess(c->pfs, 1);
                c->motorOn = 1;
            }
            if ((osGetTime() - c->rumbleCheckTime) * 64 / 3000 > 1000000) {
                Rumble_Restart(channel);
            }
            c->rumbleCount++;
            if (c->rumbleCount >= c->rumblePeriod) {
                c->rumbleCount = 0;
            } else {
                c->status = __osMotorAccess(c->pfs, 0);
                c->motorOn = 0;
            }
            c->rumbleFrames--;
        } else {
            if (c->motorOn) {
                c->status = __osMotorAccess(c->pfs, 0);
                c->motorOn = 0;
            }
            if ((osGetTime() - c->idleCheckTime) * 64 / 3000 > 100000) {
                Rumble_Stop(channel);
            }
        }
    }
}
