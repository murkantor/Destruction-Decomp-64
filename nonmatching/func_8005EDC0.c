typedef struct {
    u8 pad0[4];
    s32 x4;
    u8 pad8[0x20];
} Ent;
typedef struct {
    s32 x0;
    u8 pad4[0x20];
} Slot;
typedef struct {
    u8 pad0[0x8938];
    Slot slots[15];
    u8 pad8B54[0xE];
    s16 x8B62;
} Mgr;
void func_8005F7A0(Mgr *, Ent *);

void func_8005EDC0(Mgr *m, Ent *e, u32 size) {
    u32 i;
    s32 j;
    u32 n = size / 40;

    for (i = 0; i < n; i++, e++) {
        for (j = 0; j < m->x8B62; j++) {
            if (e->x4 == m->slots[j].x0) {
                break;
            }
        }
        if (j == m->x8B62) {
            func_8005F7A0(m, e);
        }
    }
}
