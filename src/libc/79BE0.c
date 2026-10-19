#include "common.h"
#include "m2c.h"

extern f64 D_800BC748;
extern f64 D_800BC750;
extern f64 D_800BC758;
extern char D_800BC740[];
extern double __kernel_standard(s32, char *, double, double, double);
extern double atan2(double, double);
double sqrt(double);
double asin(double x) {
    if ((x > D_800BC748) | (x < D_800BC750)) {
        return __kernel_standard(1, D_800BC740, x, 0.0, 0.0);
    }
    return atan2(x, sqrt(D_800BC758 - x * x));
}
