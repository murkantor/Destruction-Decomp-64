#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x1058]; s32 x1058; } Game;
extern void *gUi[];
extern Game *gGame;
extern s8 D_800D31CC;
extern s32 D_8009735C[];
extern f32 D_80125BD4;
void Ui_RemoveElement(void *, s32);
void Ui_AddElement(void *, s32, s32);
void func_8005D8B8(void *, s32, s32);
void Msg_Queue(s32, s32, s32, s32);
f32 Time_ToSeconds(void *);

void func_800457E4(void) {
    Ui_RemoveElement(gUi[0], 0x105);
    Ui_RemoveElement(gUi[0], 0x108);
    Ui_AddElement(gUi[0], 2, 0x106);
    Ui_AddElement(gUi[0], 2, 0x109);
    if (D_800D31CC == 3) {
        func_8005D8B8(gUi[0], 2, 0x25);
    }
    Msg_Queue(0xF7, 0, 1, 0);
    D_80125BD4 = Time_ToSeconds(&gGame->x1058) + 1.0;
    D_8009735C[0]++;
}
