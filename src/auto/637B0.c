#include "common.h"
#include "m2c.h"

extern u64 D_800C06F8;
extern u64 D_800C0700;
extern f32 D_800C0708;
extern s32 D_800C070C;

u64 osGetTime(void);
s32 Timer_GetRate(void);
#ifdef TARGET_PC
void Port_WaitUntilNs(u64 ns);
#endif

void Fps_Update(void) {
    u64 now;
    u64 x;
    u64 delta;
    s32 v;

#ifdef TARGET_PC
    /* The frame count below rounds half a frame up, so a frame shorter than
       1/rate s (never on the N64, easy on a PC) would run the game fast:
       wait out the rest of the frame first */
    if (Timer_GetRate() > 0) {
        Port_WaitUntilNs(D_800C06F8 + 1000000000ULL / Timer_GetRate());
    }
#endif
    now = (osGetTime() * 64) / 3;
    delta = now - D_800C06F8;
    x = delta * Timer_GetRate();
    v = x * 1e-9 + 0.5;
    D_800C070C = v;
    if (Timer_GetRate() > 0) {
        D_800C0708 = (f32)(u32)v / Timer_GetRate();
    } else {
        D_800C0708 = 0.0f;
    }
    D_800C06F8 = now;
    D_800C0700 = D_800C0708 * 1000000000.0;
}
