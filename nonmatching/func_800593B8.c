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

extern s32 D_800B0BA0;

void func_800593B8(void) {
    s32 i;
    s32 j;
    Controller *c;

    if (D_800B0BA0) {
        osContStartReadData(&gSiMesgQueue);
        D_800B0BA0 = 0;
    }
    while (osRecvMesg(&gSiMesgQueue, NULL, OS_MESG_NOBLOCK) != 0) {
    }
    osContGetReadData(gContPads);
    gNumControllers = 0;
    for (i = 0; i < 4; i++) {
        if (((gContPattern >> i) & 1) && gContPads[i].errnum == 0) {
            gNumControllers++;
            Pad_Update(i);
        } else {
            Pad_Clear(i);
        }
    }
    gNumControllers = 0;
    for (i = 0, c = gControllers; i < 4; i++, c++) {
        if (gRumbleRetry) {
            c->status = osMotorInit(&gSiMesgQueue, c->pfs, i);
            if (c->status == 0) {
                for (j = 0; j < 3; j++) {
                    c->status = __osMotorAccess(c->pfs, 0);
                }
            }
        }
        c->idleCheckTime = osGetTime();
        c->rumbleFrames = 0;
        c->rumbleCount = 0;
        c->rumblePeriod = 0;
        c->motorOn = 0;
    }
    osContStartReadData(&gSiMesgQueue);
}
