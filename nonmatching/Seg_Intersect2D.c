typedef struct { f32 x0; f32 x4; f32 x8; f32 xC; f32 x10; f32 x14; } Seg;
u8 func_80001670(f32, f32);

s32 Seg_Intersect2D(Seg *a, Seg *b) {
    f32 ax;
    f32 cz;
    f32 bx;
    f32 bz;
    f32 dz;
    f32 cx;
    f32 dx;
    f32 az;
    f32 d1, d2, k, e, f;

    dx = b->xC;
    dz = b->x14;
    cx = b->x0;
    bx = a->xC;
    cz = b->x8;
    bz = a->x14;
    ax = a->x0;
    az = a->x8;

    k = bx * az - ax * bz;
    d1 = (bz - az) * cx + (ax - bx) * cz + k;
    d2 = (bz - az) * dx + (ax - bx) * dz + k;
    if (d1 != 0.0f && d2 != 0.0f) {
        if (func_80001670(d1, d2)) return 0;
    }
    k = dx * cz - cx * dz;
    d1 = (dz - cz) * ax + (cx - dx) * az + k;
    d2 = (dz - cz) * bx + (cx - dx) * bz + k;
    if (d1 != 0.0f && d2 != 0.0f) {
        if (func_80001670(d1, d2)) return 0;
    }
    return 1;
}
/* agent45: 15 words, FP temp regs only (k gets f10 vs f6). Tried decl/load order hill-climb, operand orders, e/f locals, inline helper. */
