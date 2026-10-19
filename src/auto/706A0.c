#include "common.h"
#include "m2c.h"

typedef struct { s32 status; } OSPfs;
s32 __osGetId(OSPfs *);
s32 func_8006FAA0(OSPfs *pfs) {
    s32 ret;

    if (pfs->status & 5) {
        ret = __osGetId(pfs);
        if (ret == 0) {
            pfs->status &= ~4;
        }
    } else {
        ret = 5;
    }
    return ret;
}
