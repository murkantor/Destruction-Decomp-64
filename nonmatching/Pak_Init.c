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
extern s32 gNumControllers;
u64 osGetTime(void);
s32 osMotorInit(OSMesgQueue *, void *, s32);
s32 __osMotorAccess(void *, s32);
s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);
s32 osContStartReadData(OSMesgQueue *);
void osContGetReadData(OSContPad *);
void Pad_Update(s32);
void Pad_Clear(s32);

extern s32 gPakReady[];
extern u8 gPakFs[];
s32 osPfsInitPak(OSMesgQueue *, void *, s32);

s32 Pak_Init(OSMesgQueue *mq) {
    s32 tries = 5;
    s32 ret;

    gPakReady[0] = 0;
    do {
        ret = osPfsInitPak(mq, gPakFs, 0);
        tries--;
    } while (ret == 1 && tries != 0);
    switch (ret) {
        case 2:
            ret = osPfsInitPak(mq, gPakFs, 0);
            if (ret != 0) {
                goto rumble;
            }
        case 0:
            gPakReady[0] = 1;
            return ret;
        rumble:
            gControllers[0].status = osMotorInit(mq, gControllers[0].pfs, 0);
            break;
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            break;
    }
    return ret;
}
