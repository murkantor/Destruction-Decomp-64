#include "common.h"
#include "m2c.h"

struct exception {
    s32 type;
    char *name;
    f64 arg1;
    f64 arg2;
    f64 retval;
};
s32 matherr(struct exception *);
extern s32 errno;

f64 __kernel_standard(s32 type, char *name, f64 arg1, f64 arg2, f64 retval) {
    struct exception e;

    e.type = type;
    e.name = name;
    e.arg1 = arg1;
    e.arg2 = arg2;
    e.retval = retval;
    if (!matherr(&e)) {
        errno = (type == 3 || type == 4) ? 34 : 33;
    }
    return e.retval;
}
