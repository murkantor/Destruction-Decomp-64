#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[4]; void *x4; s32 x8; u8 *xC; s32 x10; } ZipFile;
extern u8 D_800B0494[];
s32 RomFile_Seek(void *, s32, s32);
s32 RomFile_Read(void *, s32, s32, void *);
void *Mem_AllocAlt(s32);
void Mem_Free(void *);
s32 memcmp(void *, void *, s32);
void *memcpy(void *, const void *, u32);

static inline s32 findSig(u8 *buf, s32 size, s32 *out) {
    s32 i;

    for (i = size - 22; i >= 0; i--) {
        if (memcmp(buf + i, D_800B0494, 4) == 0) {
            *out = i;
            return 1;
        }
    }
    return 0;
}

s32 Zip_ReadEndOfCentralDir(ZipFile *zip) {
    s32 size = 0x400;
    u8 *buf;
    s32 off;

    do {
        if (zip->x8 < size) {
            size = zip->x8;
        }
        if (RomFile_Seek(zip->x4, zip->x8 - size, 0) != 0) {
            return -1;
        }
        buf = Mem_AllocAlt(size);
        if (buf == NULL) {
            break;
        }
        if (RomFile_Read(buf, size, 1, zip->x4) != 1) {
            Mem_Free(buf);
            return -1;
        }
        if (findSig(buf, size, &off)) {
            zip->x10 = size - off;
            zip->xC = Mem_AllocAlt(zip->x10);
            if (zip->xC == NULL) {
                Mem_Free(buf);
                return -1;
            }
            memcpy(zip->xC, buf + off, zip->x10);
            Mem_Free(buf);
            return 0;
        }
        Mem_Free(buf);
    } while (size < zip->x8 && (size <<= 1, 1));
    return -1;
}
