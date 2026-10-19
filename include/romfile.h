#ifndef ROMFILE_H
#define ROMFILE_H

#include "ultra64.h"

/* A read-only file over a range of ROM, used by the zip reader for zippeddata.zip. */
typedef struct RomFile {
    /* 0x0 */ u32 start;  /* ROM address of the first byte */
    /* 0x4 */ u32 end;    /* ROM address after the last byte */
    /* 0x8 */ u32 pos;    /* current ROM address */
} RomFile;

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

RomFile *RomFile_Open(const char *name, const char *mode);
s32 RomFile_Read(u8 *buf, u32 size, s32 count, RomFile *f);
s32 RomFile_Seek(RomFile *f, s32 offset, s32 whence);
s32 RomFile_Tell(RomFile *f);
s32 RomFile_Close(RomFile *f);

#endif
