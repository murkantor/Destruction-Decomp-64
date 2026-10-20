#include "common.h"
#include "m2c.h"

M2C_UNK Fade_Init();                            /* extern */
M2C_UNK Ui_AddElement(s32, M2C_UNK, M2C_UNK);       /* extern */
M2C_UNK Clock_Reset(s32);                         /* extern */
M2C_UNK Clock_Resume(s32);                         /* extern */
M2C_UNK Clock_Pause(s32);                         /* extern */
extern s32 gGame;
extern s32 gUi;

void Logo_LookingGlassInit(void) {
    Ui_AddElement(gUi, 2, 0x1F);
    Fade_Init();
    Clock_Pause(gGame + 0x10A8);
    Clock_Reset(gGame + 0x10A8);
    Clock_Resume(gGame + 0x10A8);
}
