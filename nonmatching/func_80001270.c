typedef struct {
    u8 pad[0x10];
    s32 key;
} ObjK;

ObjK *func_80001270(ObjK *a, ObjK *b) {
    ObjK *best = a;

    if (b != NULL && !(best->key < b->key)) {
        best = b;
    }
    return best;
}
