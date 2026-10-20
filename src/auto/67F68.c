#include "common.h"
#include "m2c.h"

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

/* signed byte stored in a u8 */
static inline f32 S8(u8 x) {
    if (x & 0x80)
        return x - 0x100;
    return x;
}
void func_80067874(void *, void *, s32);
void osWritebackDCacheAll(void);

void Model_Relocate(Hdr *arg0, s32 base) {
    Hdr *h = arg0;
    s32 i;

    if (!(h->x10 & 0x80000000)) {
        h->x10 |= 0x80000000;
        func_80067874(&h->x24, h, 3);
        func_80067874(h->x2C, h, h->x20);
        for (i = 0; i < h->x20; i++) {
            f32 *pf = (f32 *)(h->x28 + i * 4);
            Node *n;
            Node *m;
            u8 v = *(u8 *)pf;
            f32 t;

            t = S8(v);
            *pf = t / 100.0;
            v = h->x24[i] - 0x30;
            t = S8(v);
            *pf += t;
            n = h->x2C[i];
            if (n->x9 == 0) {
                u32 x0 = n->x0;
                if ((x0 & 0xFF000000) != 0xFF000000) {
                    x0 += base;
                    n->x0 = x0;
                }
                h->x2C[i]->x9 = 1;
                m = h->x2C[i];
                if (m->xC != 0) {
                    m->xC += (s32)arg0;
                }
                m = h->x2C[i];
                if (m->x8 == 0) {
                    m->x10 += (s32)arg0;
                }
            }
        }
        osWritebackDCacheAll();
    }
}
