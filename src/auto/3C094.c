#include "common.h"
#include "m2c.h"

extern void *gUi[];
extern u8 *gGame;
void Ui_ClearDrawList(void *);
void Ui_AddElement(void *, s32, s32);
void Ui_RevealInit(void);
void Clock_Pause(void *);
void Clock_Reset(void *);
void Clock_Resume(void *);

void Logo_ThqInit(void) {
    Ui_ClearDrawList(gUi[0]);
    Ui_AddElement(gUi[0], 2, 30);
    Ui_RevealInit();
    Clock_Pause(gGame + 0x10A8);
    Clock_Reset(gGame + 0x10A8);
    Clock_Resume(gGame + 0x10A8);
}
