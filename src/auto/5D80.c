#include "common.h"
#include "m2c.h"

M2C_UNK func_80005814();                            /* extern */
M2C_UNK Ai_UpdateState();                            /* extern */
M2C_UNK func_80005C30();                            /* extern */
extern s32 D_800958A8;
extern s32 D_800958AC;
extern void *gGame;

void func_80005180(void) {
    if (M2C_FIELD(M2C_FIELD(gGame, void **, 0x1100), s32 *, 0x10) == 0) {
        func_80005C30();
        return;
    }
    if ((D_800958A8 == 3) && (D_800958AC == 1)) {
        func_80005814();
        return;
    }
    Ai_UpdateState();
}
