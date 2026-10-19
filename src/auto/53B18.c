#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    void *x4;
} Arc;
typedef struct {
    u8 pad0[0x2C];
    s32 x2C;
} ArcEntry;
extern char D_800BB808[];
void *RomFile_Open(s32, char *);
s32 RomFile_Seek(void *, s32, s32);
s32 RomFile_Read(void *, s32, s32, void *);

static inline Arc *Arc_Open(Arc *arc) {
    arc->x4 = RomFile_Open(arc->x0, D_800BB808);
    return arc->x4 ? arc : NULL;
}

s32 Arc_SeekToEntryData(Arc *arc, ArcEntry *e) {
    u8 hdr[30];
    u16 n;
    u16 m;

    if (arc->x4 == NULL && Arc_Open(arc) == NULL) {
        return -1;
    }
    if (RomFile_Seek(arc->x4, e->x2C, 0) != 0) {
        return -1;
    }
    if (RomFile_Read(hdr, 30, 1, arc->x4) != 1) {
        return -1;
    }
    n = (hdr[27] << 8) | hdr[26];
    m = (hdr[29] << 8) | hdr[28];
    if (RomFile_Seek(arc->x4, e->x2C + 30 + n + m, 0) != 0) {
        return -1;
    }
    return 0;
}
