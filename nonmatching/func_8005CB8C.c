typedef struct {
    u8 pad[0x40];
    s32 x40;
    u8 pad44[0xC];
} Slot;
typedef struct {
    u8 pad[0x4C4];
    Slot *bufs[4];
    s32 counts[4];
} Owner;
extern s8 D_800D31CC;
void *Mem_Alloc(u32);

void func_8005CB8C(Owner *o) {
    s32 i;
    s32 j;
    Slot *s;

    for (i = 0; i < D_800D31CC; i++) {
        s = o->bufs[i] = Mem_Alloc(60 * sizeof(Slot));
        for (j = 0; j < 60; j++, s++) {
            s->x40 = 0;
        }
        o->counts[i] = 0;
    }
}
