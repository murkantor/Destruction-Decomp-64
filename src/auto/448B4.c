#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x1058]; u64 x1058; } Game;
typedef struct { u64 x0; u8 pad8[5]; u8 xD; } Clock;
typedef struct { u8 timer[0xC]; s32 xC; s32 x10; u8 pad[0x68 - 0x14]; } Elem;
typedef struct { s32 x0; u8 pad[0x50]; Elem e[4]; } Group;
extern Game *gGame;
extern void *gUi;
extern u8 D_80097354;
extern u8 D_80097334[];
extern Clock D_8007EF20;
extern Group D_800958A4;
void func_8005DC68(void *);
void Ui_AddElement(void *, s32, s32);
void Msg_Queue(s32, s32, s32, s32);
u64 Time_Get(u64 *);
void Timer_Start(void *, s64);

void func_80043CB4(void) {
    s32 i;

    if (D_80097354) {
        func_8005DC68(gUi);
        Ui_AddElement(gUi, 2, 0x20);
    }
    Ui_AddElement(gUi, 2, 0x24);
    D_8007EF20.xD = 0;
    D_8007EF20.x0 = Time_Get(&gGame->x1058) + 52000000.0;
    if (D_80097334[0]) {
        Msg_Queue(0xD2, 0x1F4, 1, 0);
        D_80097334[0] = 0;
    }
    D_800958A4.x0 = 4;
    for (i = 0; i < 4; i++) {
        D_800958A4.e[i].xC = 2;
        D_800958A4.e[i].x10 = 0;
        Timer_Start(D_800958A4.e[i].timer, 1000000000);
    }
}
