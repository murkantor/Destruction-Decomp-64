#include "common.h"
#include "m2c.h"

M2C_UNK alSynNew();                            /* extern */
extern s32 alGlobals;

void func_8006B060(s32 arg0) {
    if (alGlobals == 0) {
        alGlobals = arg0;
        alSynNew();
    }
}

M2C_UNK alSynDelete();                            /* extern */

void func_8006B08C(void) {
    if (alGlobals != 0) {
        alSynDelete();
        alGlobals = 0;
    }
}
