#include "common.h"
#include "m2c.h"

typedef struct {
    char *name;
    void *file;
    s32 size;
    u8 *eocd;
    u8 pad10[4];
    u8 *cdir;
    s32 x18;
    u8 pad1C[0x4C - 0x1C];
    s32 x4C;
    u32 sig;
    u16 disk;
    u16 cdDisk;
    u16 entriesDisk;
    u16 entries;
    u32 cdSize;
    u32 cdOffset;
    u16 commentLen;
    u8 pad66[2];
    u8 *comment;
} ZipFile;
extern char D_800BB808[];
void *Mem_AllocAlt(u32);
void Mem_Free(void *);
void *RomFile_Open(const char *, const char *);
s32 RomFile_Read(void *, u32, s32, void *);
s32 RomFile_Seek(void *, s32, s32);
s32 RomFile_Tell(void *);
s32 RomFile_Close(void *);
s32 Zip_ReadEndOfCentralDir(ZipFile *);
u32 strlen(const char *);
char *strcpy(char *, const char *);

#define RD32(p, o) ((p)[(o) + 3] << 24 | (p)[(o) + 2] << 16 | (p)[(o) + 1] << 8 | (p)[(o)])
#define RD16(p, o) ((p)[(o)] | (p)[(o) + 1] << 8)

ZipFile *Zip_Open(char *name) {
    ZipFile *z = Mem_AllocAlt(0x6C);
    if (z == NULL) {
        return NULL;
    }
    if ((z->file = RomFile_Open(name, D_800BB808)) == NULL) {
        goto fail0;
    }
    if (RomFile_Seek(z->file, 0, 2) != 0) {
        goto fail1;
    }
    if ((z->size = RomFile_Tell(z->file)) < 0 || z->size == 0) {
        goto fail1;
    }
    if (Zip_ReadEndOfCentralDir(z) != 0) {
        goto fail1;
    }
    z->sig = RD32(z->eocd, 0);
    z->disk = RD16(z->eocd, 4);
    z->cdDisk = RD16(z->eocd, 6);
    z->entriesDisk = RD16(z->eocd, 8);
    z->entries = RD16(z->eocd, 10);
    z->cdSize = RD32(z->eocd, 12);
    z->cdOffset = RD32(z->eocd, 16);
    z->commentLen = z->eocd[21] << 8 | z->eocd[20];
    z->comment = z->eocd + 0x16;
    if (z->disk != z->cdDisk || z->entriesDisk != z->entries || z->entriesDisk == 0) {
        goto fail2;
    }
    if (RomFile_Seek(z->file, z->cdOffset, 0) != 0) {
        goto fail2;
    }
    if ((z->cdir = Mem_AllocAlt(z->cdSize)) == NULL) {
        goto fail2;
    }
    if (RomFile_Read(z->cdir, z->cdSize, 1, z->file) != 1) {
        goto fail3;
    }
    PORT_MARK_RAW(z->cdir, z->cdSize);
    z->x4C = 0;
    z->x18 = 0;
    if ((z->name = Mem_AllocAlt(strlen(name) + 1)) == NULL) {
        goto fail3;
    }
    strcpy(z->name, name);
    return z;
fail3:
    Mem_Free(z->cdir);
fail2:
    Mem_Free(z->eocd);
fail1:
    RomFile_Close(z->file);
fail0:
    Mem_Free(z);
    return NULL;
}
