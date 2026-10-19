#include "common.h"
#include "m2c.h"

s16 _Ldunscale(s16 *pex, void *px) {
    u16 *ps = (u16 *)px;
    s16 xchar = (ps[0] & 0x7FF0) >> 4;

    if (xchar == 0x7FF) {
        *pex = 0;
        return (ps[0] & 0xF || ps[1] || ps[2] || ps[3]) ? 2 : 1;
    } else if (0 < xchar) {
        ps[0] = ps[0] & ~0x7FF0 | (0x3FF << 4);
        *pex = xchar - (0x3FF - 1);
        return -1;
    }
    if (0 > xchar) {
        return 2;
    } else {
        *pex = 0;
        return 0;
    }
}
