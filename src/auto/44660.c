#include "common.h"
#include "m2c.h"

M2C_UNK Ui_AddElement(s32, M2C_UNK, M2C_UNK);       /* extern */
extern s32 gUi;

void TitleScreen_AddLogo(void) {
    Ui_AddElement(gUi, 2, 0x20);
}
