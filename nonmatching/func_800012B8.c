typedef struct {
    u8 pad[0x18];
    s32 key;
} ObjK;

ObjK *func_800012B8(ObjK *a, ObjK *b) {
    ObjK *best = a;

    if (b != NULL && !(b->key < best->key)) {
        best = b;
    }
    return best;
}
