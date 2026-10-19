#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x1058];
    u8 x1058[0x10A8 - 0x1058];
    u8 x10A8[8];
} Game;

extern Game *gGame[];
extern void *gUi;
extern s32 D_8009735C[];
extern f32 D_80125BD4;
f32 Time_ToSeconds(void *);
void Ui_AddElement(void *, s32, s32);
void Msg_Queue(s32, s32, s32, s32);
void func_80063564(s32, s32);
void func_800638C8(void *);
void func_80063890(void *);
void func_800638C0(void *);

s32 func_80044C84(void) {
    s32 ret = 0;

    if (Time_ToSeconds(gGame[0]->x10A8) >= 1.0 && D_8009735C[0] == 0) {
        D_80125BD4 = Time_ToSeconds(gGame[0]->x1058) + 1.0;
        Ui_AddElement(gUi, 2, 0x104);
        Ui_AddElement(gUi, 2, 0x107);
        Msg_Queue(0xDF, 0, 1, 0);
        D_8009735C[0]++;
    }
    if (Time_ToSeconds(gGame[0]->x1058) >= D_80125BD4 && D_8009735C[0] == 3) {
        func_80063564(0x1C, 1);
        func_800638C8(gGame[0]->x10A8);
        func_80063890(gGame[0]->x10A8);
        func_800638C0(gGame[0]->x10A8);
        D_80125BD4 += 0.5;
        ret = 1;
    }
    return ret;
}
