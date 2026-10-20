#include "common.h"
#include "m2c.h"

M2C_UNK Mem_Free(s32);                         /* extern */
void *Ui_FindElement(s32, M2C_UNK, M2C_UNK);         /* extern */
extern s32 D_800D1AEC;
extern s32 gUi;

void Fade_Free(void) {
    void *temp_v0;

    temp_v0 = Ui_FindElement(gUi, 2, 0x1F);
    Mem_Free(*M2C_FIELD(temp_v0, s32 **, 0x10));
    *M2C_FIELD(temp_v0, s32 **, 0x10) = 0;
    D_800D1AEC = 0;
}
