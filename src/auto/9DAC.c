#include "common.h"
#include "m2c.h"

M2C_UNK EngineSfx_UpdateAll();                            /* extern */
M2C_UNK func_8000721C(s32);                         /* extern */
M2C_UNK func_800078C0(s32);                         /* extern */
M2C_UNK func_80008818();                            /* extern */
extern void *gGame;

void func_800091AC(s32 arg0) {
    if (M2C_FIELD(gGame, s16 *, 0xC) == 2) {
        EngineSfx_UpdateAll();
        func_800078C0(arg0);
        func_8000721C(arg0);
    }
    func_80008818();
}
