#include "common.h"
#include "m2c.h"

extern void *gUi[];
extern u8 *gGame;
void Ui_ClearDrawList(void *);
void Ui_AddElement(void *, s32, s32);
void Clock_Pause(void *);
void Clock_Reset(void *);
void Clock_Resume(void *);

void Logo_N64Init(void) {
    Ui_ClearDrawList(gUi[0]);
    Ui_AddElement(gUi[0], 2, 0xB0);
    Ui_AddElement(gUi[0], 2, 0xC5);
    Clock_Pause(gGame + 0x10A8);
    Clock_Reset(gGame + 0x10A8);
    Clock_Resume(gGame + 0x10A8);
}
