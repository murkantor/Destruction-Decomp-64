#include "common.h"
#include "m2c.h"

M2C_UNK alSynNew();                            /* extern */
extern s32 alGlobals;

void alInit(s32 arg0, s32 arg1) {
    if (alGlobals == 0) {
        alGlobals = arg0;
        alSynNew(arg0, arg1);
    }
}

M2C_UNK alSynDelete();                            /* extern */

void alClose(s32 arg0) {
    if (alGlobals != 0) {
        alSynDelete(arg0);
        alGlobals = 0;
    }
}
