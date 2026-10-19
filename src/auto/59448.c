#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x18]; } OSIoMesg;
void osCreateMesgQueue(OSMesgQueue *, OSMesg *, s32);
void osWritebackDCacheAll(void);
void osInvalDCache(void *, s32);
void osInvalICache(void *, s32);
s32 osPiStartDma(OSIoMesg *, s32, s32, u32, void *, u32, OSMesgQueue *);
s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);

void Dma_LoadAndFlush(u8 *dst, u32 src, u32 size) {
    OSIoMesg io;
    OSMesgQueue mq;
    OSMesg msg;
    u32 rem;
    u32 chunk;
    u8 *d;
    u32 s;

    osCreateMesgQueue(&mq, &msg, 1);
    rem = size;
    osWritebackDCacheAll();
    osInvalDCache(dst, size);
    osInvalICache(dst, size);
    d = dst;
    s = src;
    while (rem != 0) {
        chunk = rem;
        if (rem > 0x2000) {
            chunk = 0x2000;
        }
        osPiStartDma(&io, 0, 0, s, d, chunk, &mq);
        osRecvMesg(&mq, NULL, 1);
        d += chunk;
        rem -= chunk;
        s += chunk;
    }
    osInvalDCache(dst, size);
    osInvalICache(dst, size);
}
