typedef struct {
    u32 x0;
    u8 pad4[4];
    u8 x8;
    u8 x9;
    u8 padA[2];
    s32 xC;
    s32 x10;
} Node;

typedef struct {
    u8 pad[0x10];
    s32 x10;
    u8 pad14[0xC];
    s32 x20;
    u8 *x24;
    u8 *x28;
    Node **x2C;
} Hdr;

static inline f32 S8(s32 x) { if (x & 0x80) return (x & 0xFF) - 0x100; return x & 0xFF; }
void func_80067874(void *, void *, s32);
void osWritebackDCacheAll(void);

void func_80067368(Hdr *arg0, s32 base) {
    Hdr *h = arg0;
    s32 i;


    if (!(h->x10 & 0x80000000)) {
        h->x10 |= 0x80000000;
        func_80067874(&h->x24, h, 3);
        func_80067874(h->x2C, h, h->x20);
        for (i = 0; i < h->x20; i++) {
            f32 *pf = (f32 *)(h->x28 + i * 4);
            s32 v = *(u8 *)pf;
            *pf = S8(v) / 100.0;
            v = h->x24[i] - 0x30;
            *pf += S8(v);
            if (h->x2C[i]->x9 == 0) {
                Node *n = h->x2C[i];
                if ((n->x0 & 0xFF000000) != 0xFF000000) {
                    n->x0 += base;
                }
                h->x2C[i]->x9 = 1;
                n = h->x2C[i];
                if (n->xC != 0) {
                    n->xC += (s32)arg0;
                }
                n = h->x2C[i];
                if (n->x8 == 0) {
                    n->x10 += (s32)arg0;
                }
            }
        }
        osWritebackDCacheAll();
    }
}
