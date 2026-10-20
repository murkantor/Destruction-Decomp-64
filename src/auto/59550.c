#include "common.h"
#include "m2c.h"

typedef struct { u16 type; u8 status; u8 errno; } OSContStatus;
extern OSMesgQueue gSiMesgQueue;
extern OSMesg D_80125F40[];
extern u8 gContPattern;
extern OSContStatus gContStatus[4];
extern s32 gNumControllers;
void osCreateMesgQueue(OSMesgQueue *, OSMesg *, s32);
void osSetEventMesg(s32, OSMesgQueue *, OSMesg);
s32 osContInit(OSMesgQueue *, u8 *, OSContStatus *);
void osContReset(OSMesgQueue *, OSContStatus *);
void Pad_Init(s32, s32);

void Pad_InitAll(void) {
    s32 i;
    s32 rumble;

    osCreateMesgQueue(&gSiMesgQueue, D_80125F40, 4);
    osSetEventMesg(5, &gSiMesgQueue, (OSMesg)0xDEAD);
    osContInit(&gSiMesgQueue, &gContPattern, gContStatus);
    osContReset(&gSiMesgQueue, gContStatus);
    gNumControllers = 0;
    for (i = 0; i < 4; i++) {
        rumble = 0;
        if ((gContPattern >> i) & 1) {
            if (gContStatus[i].type == 5) {
                rumble = 1;
                gNumControllers++;
            }
            Pad_Init(i, rumble);
        }
    }
}
