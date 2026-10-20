#include "common.h"
#include "romfile.h"

extern RomFile gRomFile;
extern u8 D_DCAF50[]; /* start of zippeddata.zip in ROM */
extern u8 D_F94840[]; /* end of the zip data, rounded up */

void Dma_LoadAndFlush(void *dst, u32 romAddr, u32 size); /* copies from ROM by DMA */
void *memcpy(void *dst, const void *src, u32 size);

/* There is only one file: the name and mode are ignored. */
RomFile *RomFile_Open(const char *name, const char *mode) {
    gRomFile.start = (u32)D_DCAF50;
    gRomFile.end = (u32)D_F94840;
    gRomFile.pos = (u32)D_DCAF50;
    return &gRomFile;
}

/* Like fread. DMA needs an 8-byte aligned buffer and an even ROM address and
 * length, so anything else goes through a buffer on the stack. */
s32 RomFile_Read(u8 *buf, u32 size, s32 count, RomFile *f) {
#ifdef TARGET_PC
    PORT_DMA_BUF(u8, tmp, 0x0000); /* 0x2000 bytes */
#define TMP_SIZE 0x2000
#else
    u8 tmp[0x2000];
#define TMP_SIZE sizeof(tmp)
#endif
    s32 left = (f->end - f->pos) / size;
    s32 bytes;

    if (left < count) {
        count = left;
    }
    bytes = size * count;
    if (((u32)buf & 7) == 0 && (f->pos & 1) == 0 && (bytes & 1) == 0) {
        Dma_LoadAndFlush(buf, f->pos, bytes);
        f->pos += bytes;
    } else {
        while (bytes != 0) {
            s32 odd = f->pos & 1;
            s32 chunk = TMP_SIZE - odd;

            if (bytes < chunk) {
                chunk = bytes;
            }
            if ((chunk + odd) & 1) {
                Dma_LoadAndFlush(tmp, f->pos - odd, chunk + odd + 1);
            } else {
                Dma_LoadAndFlush(tmp, f->pos - odd, chunk + odd);
            }
            bytes -= chunk;
            memcpy(buf, tmp + odd, chunk);
            buf += chunk;
            f->pos += chunk;
        }
    }
    return count;
}
#undef TMP_SIZE

s32 RomFile_Seek(RomFile *f, s32 offset, s32 whence) {
    switch (whence) {
        case SEEK_CUR:
            f->pos = f->pos + offset;
            break;
        case SEEK_END:
            f->pos = f->end - offset;
            break;
        case SEEK_SET:
            f->pos = f->start + offset;
            break;
    }
    return 0;
}

s32 RomFile_Tell(RomFile *f) {
    return f->pos - f->start;
}

s32 RomFile_Close(RomFile *f) {
    f->start = 0;
    f->end = 0;
    f->pos = 0;
    return 0;
}
