typedef struct {
    f32 x, y, z;
} V3;

void func_80031990(V3 *a, V3 *b, s32 n) {
    s32 i;

    for (i = 0; i < n; i++) {
        a[i].x += b->x;
        a[i].y += b->y;
        a[i].z += b->z;
    }
}
