#include "common.h"
#include "m2c.h"

M2C_UNK alLink(s32, void *);                 /* extern */
M2C_UNK alUnlink(s32);                         /* extern */

void _collectPVoices(void *arg0) {
    s32 var_s0;
    void *var_s1;

    var_s1 = arg0;
    var_s0 = M2C_FIELD(var_s1, s32 *, 0x14);
    if (var_s0 != 0) {
        do {
            alUnlink(var_s0);
            alLink(var_s0, var_s1 + 4);
            var_s0 = M2C_FIELD(var_s1, s32 *, 0x14);
        } while (var_s0 != 0);
    }
}
