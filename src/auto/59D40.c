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

void Pad_Rumble(s32 channel, s32 frames, f32 strength) {
    Controller *c = &gControllers[channel];
    f32 f;
    u32 period;

    if (c->type == 1) {
        if (strength > 0.1f) {
            f = strength * 10.0f;
            if (!(f <= 10.0f)) {
                f = 10.0f;
            }
            period = 10.0f - f;
            c->rumbleFrames = frames;
            c->rumblePeriod = period;
        } else {
            c->rumbleFrames = 0;
            c->rumblePeriod = -1;
            Rumble_AllowRetry();
        }
    }
}
