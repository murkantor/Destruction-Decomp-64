#include "common.h"
#include "m2c.h"

extern u8 D_80096EBC;
extern s32 D_80096EC0;

s32 func_800458B8(void) {
    s32 var_v0;

    var_v0 = 0;
    if (D_80096EC0 == -1) {
        if (D_80096EBC != 0) {
            return 0;
        }
        D_80096EBC = 1;
        var_v0 = 1;
        /* Duplicate return node #5. Try simplifying control flow for better match */
        return var_v0;
    }
    return var_v0;
}
