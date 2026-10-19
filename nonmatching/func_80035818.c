static inline s32 side(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4) {
    f32 c;
    f64 d1;
    f64 d2;

    c = x2 * y1 - x1 * y2;
    d1 = (y2 - y1) * x3 + (x1 - x2) * y3 + c;
    d2 = (y2 - y1) * x4 + (x1 - x2) * y4 + c;
    if (d1 == 0.0) {
        return 1;
    }
    if (d2 == 0.0) {
        return 1;
    }
    if (d1 < 0.0) {
        if (d2 < 0.0) {
            return 0;
        }
    }
    if (0.0 < d1) {
        if (0.0 < d2) {
            return 0;
        }
    }
    return 1;
}

s32 func_80035818(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4) {
    if (side(x1, y1, x2, y2, x3, y3, x4, y4) == 0) {
        return 0;
    }
    if (side(x3, y3, x4, y4, x1, y1, x2, y2) == 0) {
        return 0;
    }
    return 1;
}
