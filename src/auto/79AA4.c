#include "common.h"
#include "m2c.h"

typedef struct { s64 quot; s64 rem; } lldiv_t;

lldiv_t lldiv(s64 num, s64 denom) {
    lldiv_t ret;

    ret.quot = num / denom;
    ret.rem = num - denom * ret.quot;
    if (ret.quot < 0 && ret.rem > 0) {
        ret.quot++;
        ret.rem -= denom;
    }
    return ret;
}
