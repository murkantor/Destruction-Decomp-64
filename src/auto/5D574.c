#include "common.h"
#include "m2c.h"

typedef struct {
    u8 data[0x18];
} OSIoMesg;
extern OSMesgQueue D_80125C20;
void osInvalDCache(void *, s32);
s32 osPiStartDma(OSIoMesg *, s32, s32, u32, void *, u32, OSMesgQueue *);
s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);

void Rom_Copy(u32 rom, u8 *dram, s32 size) {
    OSIoMesg mb;
    OSMesg msg;
    s32 chunk;

    while (size != 0) {
        chunk = size;
        if (chunk > 0x4000) {
            chunk = 0x4000;
        }
        osInvalDCache(dram, chunk);
        osPiStartDma(&mb, 0, 0, rom, dram, chunk, &D_80125C20);
        osRecvMesg(&D_80125C20, &msg, OS_MESG_BLOCK);
        rom += chunk;
        size -= chunk;
        dram += chunk;
    }
}
