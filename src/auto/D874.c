#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x1058]; u64 x1058; } Game;
extern Game *gGame;
extern void *gUi;
extern f32 D_80125BD4;
extern s32 D_8009735C[];
f32 Time_ToSeconds(void *);
void Ui_RemoveElement(void *, s32);
void Ui_AddElement(void *, s32, s32);
void Msg_Queue(s32, s32, s32, s32);

s32 func_8000CC74(void) {
    f32 t = Time_ToSeconds(&gGame->x1058);
    s32 ret = 0;

    if (D_80125BD4 <= t) {
        if (D_8009735C[0] == 1) {
            Ui_RemoveElement(gUi, 0x104);
            Ui_RemoveElement(gUi, 0x107);
            Ui_AddElement(gUi, 2, 0xA1);
            Ui_AddElement(gUi, 2, 0xA2);
            Msg_Queue(0xF5, 0, 1, 0);
            D_80125BD4 = t + 1.0;
            D_8009735C[0]++;
        } else if (D_8009735C[0] == 2) {
            Ui_RemoveElement(gUi, 0xA1);
            Ui_RemoveElement(gUi, 0xA2);
            Ui_AddElement(gUi, 2, 0x105);
            Ui_AddElement(gUi, 2, 0x108);
            Msg_Queue(0xF6, 0, 1, 0);
            D_80125BD4 = t + 1.0;
            D_8009735C[0]++;
        } else if (D_8009735C[0] == 4) {
            Ui_RemoveElement(gUi, 0x106);
            Ui_RemoveElement(gUi, 0x109);
            ret = 1;
            D_8009735C[0]++;
        }
    }
    return ret;
}
