#include "common.h"
#include "m2c.h"

typedef struct { u16 button; s8 stick_x; s8 stick_y; u8 errno; u8 pad; } ContPad;
extern s32 D_800B0BA0;
extern OSMesgQueue gSiMesgQueue;
extern ContPad gContPads[];
extern u8 gContPattern;
extern s32 gNumControllers;
void osContStartReadData(OSMesgQueue *);
s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);
void osContGetReadData(ContPad *);
void Pad_Update(s32);
void Pad_Clear(s32);

void Cont_ReadAll(void) {
    s32 i;

    if (D_800B0BA0) {
        osContStartReadData(&gSiMesgQueue);
        D_800B0BA0 = 0;
    }
    while (osRecvMesg(&gSiMesgQueue, NULL, 0) != 0) {
    }
    osContGetReadData(gContPads);
    gNumControllers = 0;
    for (i = 0; i < 4; i++) {
        if (((gContPattern >> i) & 1) && gContPads[i].errno == 0) {
            gNumControllers++;
            Pad_Update(i);
        } else {
            Pad_Clear(i);
        }
    }
}
