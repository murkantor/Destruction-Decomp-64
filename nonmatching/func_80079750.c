f64 __kernel_standard();
f64 frexp(f64, s32 *);
f64 func_80079660(f64);

f64 func_80079750(f64 x) {
    f64 r;
    s32 e;

    if (x < 0.0) {
        return __kernel_standard(1, "log", x, 0.0, 0.0);
    }
    if (x == 0.0) {
        ((u32 *)&r)[0] = 0xFFF00000;
        ((u32 *)&r)[1] = 0;
        return __kernel_standard(3, "log", x, 0.0, r);
    }
    if (x == 1.0) {
        return 0.0;
    }
    r = func_80079660(frexp(x, &e));
    r = e * 0.693147180559945286 + r;
    return r;
}
