#include "common.h"
#include "m2c.h"

M2C_UNK alSynNew();                            /* extern */
extern s32 alGlobals;

void alInit(s32 arg0) {
    if (alGlobals == 0) {
        alGlobals = arg0;
        alSynNew();
    }
}

M2C_UNK alSynDelete();                            /* extern */

void alClose(void) {
    if (alGlobals != 0) {
        alSynDelete();
        alGlobals = 0;
    }
}
