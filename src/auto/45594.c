#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xB4]; u8 xB4; } Car44994;
typedef struct { Car44994 *x0; u8 pad[0x64]; } Slot44994;
extern u8 *gGame;
typedef struct { f32 x0; f32 x4; f32 x8; } Timing44994;

extern Timing44994 D_800D1BA0;
extern f32 D_800D1BA8; extern volatile f32 D_800D1BA4;
extern u8 D_800D1BAC;
extern s8 D_800D31CC;
extern Slot44994 D_800958DC[];
f32 Time_ToSeconds(void *);
s8 Announcer_GetQueueCount(void);
u8 Game_TimeBefore(void);
void Camera_UpdateChase(Car44994 *);
void func_80063564(s32, s32);

extern f64 D_800BAA70; extern volatile f64 D_800BAA78;
s32 func_80044994(void) {
    f32 t;
    f32 r;
    s32 i;

    t = Time_ToSeconds(gGame + 0x1058);
    D_800D1BA8 = (t - D_800D1BA0.x0) * D_800BAA70;
    D_800D1BA4 = t;
    if (D_800D1BA8 >= D_800BAA78) {
        D_800D1BA8 = 1.0f;
    }
    if (!D_800D1BAC) {
        return 0;
    }
    if (Announcer_GetQueueCount()) {
        return 0;
    }
    if (Game_TimeBefore()) {
        return 0;
    }
    for (i = 0; i < D_800D31CC; i++) {
        D_800958DC[i].x0->xB4 = 2;
        Camera_UpdateChase(D_800958DC[i].x0);
    }
    func_80063564(0x1B, 1);
    return 0;
}
