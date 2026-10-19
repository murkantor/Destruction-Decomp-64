#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; u8 *x10; } Bank;
typedef struct { u8 pad[0x70]; Bank *x70; } SeqPlayer;
void Seq_ReadEnvelope(SeqPlayer *, u8 *);

u8 *func_800651B8(SeqPlayer *obj, u8 *src) {
    s32 v = *src++;

    if (v & 0x80) {
        v &= 0x7F; v <<= 8; v |= *src++;
    }
    Seq_ReadEnvelope(obj, obj->x70->x10 + v * 7);
    return src;
}
