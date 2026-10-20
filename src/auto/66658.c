#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    s32 *x4;
    s32 *x8;
    s32 *xC;
} SeqHdr;

typedef struct {
    s32 x0;
    u8 pad4[0x30 - 0x4];
    s32 x30;
    s32 x34;
    u8 pad38[0x40 - 0x38];
    s32 x40;
    u8 pad44[0x70 - 0x44];
    SeqHdr *x70;
    u8 pad74[0x78 - 0x74];
    s32 x78;
    u8 pad7C[0x80 - 0x7C];
    s32 x80;
    s32 x84;
    u8 pad88[0x120 - 0x88];
} Voice;

extern Voice *gVoices;
extern s32 D_800CCAB8;
void Mem_RelocatePtrs(void *p, void *base, s32 n);
s32 Voice_FindFree(SeqHdr *h, s32 i);
void Seq_ResetVoice(Voice *v);

s32 Seq_StartVoices(SeqHdr *h) {
    s32 n = h->x0;
    s32 i;
    s32 id;
    Voice *v;
    s32 val;

    if ((u32)h->x4 < 0x400) {
        Mem_RelocatePtrs(&h->x4, h, 5);
        Mem_RelocatePtrs(h->x4, h, n);
        Mem_RelocatePtrs(h->x8, h, n);
        Mem_RelocatePtrs(h->xC, h, n);
    }
    id = D_800CCAB8++;
    for (i = 0; i < n; i++) {
        if (h->x4[i] != 0) {
            v = &gVoices[Voice_FindFree(h, i)];
            Seq_ResetVoice(v);
            v->x70 = h;
            v->x34 = v->x84 = h->x8[i];
            v->x30 = v->x80 = h->xC[i];
            val = h->x4[i];
            v->x40 = id;
            v->x0 = v->x78 = val;
        }
    }
    return id;
}
