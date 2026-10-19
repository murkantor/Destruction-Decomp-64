typedef struct { u8 pad0[4]; s32 x4; u8 pad8[4]; f32 xC; f32 x10; f32 x14; f32 x18; u8 pad1C[0x3C]; } Spline;

void func_8001CCA0(Spline *out, Spline *in) {
    f32 x;
    f32 y;
    f32 a;
    f32 b;
    f32 x2;
    f32 y2;
    f32 x3;
    f32 y3;
    f32 y4;
    f32 x4;
    f32 inv;
    f32 t;

    *out = *in;
    x = in->x4;
    y = in->x14;
    a = in->xC;
    b = in->x10;
    x2 = x * x;
    y2 = y * y;
    x3 = x2 * x;
    y3 = y2 * y;
    y4 = y3 * y;
    x4 = x3 * x;
    t = -3.0f * x2;
    inv = 1.0f / (x2 * y3 * (y2 - 2.0f * x * y + x2));
    out->x18 = a;
    out->xC = (2.0f * b * x3 - (a * t * y + 2.0f * a * x3 + a * y3 + b * (3.0f * x2) * y)) * inv;
    out->x10 = 2.0f * (a * (-2.0f * x2) * y2 + a * x4 + a * y4 + 2.0f * b * x2 * y2 - b * x4) * inv;
    out->x14 = (b * (3.0f * x4) - (a * (-4.0f * x3) * y + a * (3.0f * x4) + a * y4 + b * (4.0f * x3) * y)) * inv * y3 / y2;
}
