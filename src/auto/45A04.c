#include "common.h"
#include "m2c.h"

extern u8 *gGame[];
extern void *gUi;
extern u8 D_800D2E58;
extern s8 D_800D31CC;
extern s32 D_8009735C;
extern s32 D_8011EEF0;
extern u8 D_800958A1;
extern u32 D_800958B4;
extern s32 D_800958A8;
extern s32 D_800958A4;
void Clock_Pause(void *);
void Clock_Reset(void *);
void Clock_Resume(void *);
void View_SetupSplitScreen(s32, s32, s32);
void func_8005F884(void *);
void Ui_SetElementColor(s32, s32);

void func_80044E04(void) {
    s32 i;

    Clock_Pause(gGame[0] + 0x10A8);
    Clock_Reset(gGame[0] + 0x10A8);
    Clock_Resume(gGame[0] + 0x10A8);
    D_8009735C = 0;
    if (D_800D2E58 != D_800D31CC) {
        if ((D_800D2E58 = D_800D31CC) == 0) {
            D_800D2E58 = 1;
        }
        View_SetupSplitScreen(D_8011EEF0, D_800D2E58, D_800958A1);
    }
    func_8005F884(gUi);
    if (D_800958B4 >= 4 && D_800958A8 != 2) {
        for (i = 0; i < D_800958A4; i++) {
            Ui_SetElementColor(i + 0x200, 0x111D6);
        }
    }
}
