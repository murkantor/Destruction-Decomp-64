extern s64 D_800B70E0[];

f64 func_8007AA20(f64 x) {
    s64 acc = 0x1000000000000000LL;
    u64 v = x * 1152921504606846976.0;
    s32 i;
    s64 d;
    s64 *t;
    i = 1;
    t = D_800B70E0;
loop:
    d = t[i] - v;
    if (d <= 0) {
        v = -d;
        acc += acc >> i;
    }
    i++;
    if (i < 0x36) {
        goto loop;
    }
    return acc / 1152921504606846976.0;
}
