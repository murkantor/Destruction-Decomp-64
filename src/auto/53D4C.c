#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[0x10]; u32 x10; u8 pad14[4]; u32 x18; u8 pad1C[0x14]; u8 *x30; } ZipEntry;
typedef struct { void *x0; void *x4; u8 pad8[4]; void *xC; u8 pad10[4]; void *x14; u8 pad18[0x34]; void *x4C; } Zip;
extern char D_800BBA5C[];
Zip *Zip_Open(void *);
ZipEntry *func_80052AE0(Zip *);
s32 func_80053050(Zip *, ZipEntry *, void *);
void *Mem_AllocAligned(u32, s32, s32);
void Mem_Free(void *);
void RomFile_Close(void *);
int sprintf(char *, const char *, ...);
u8 *strrchr(u8 *, int);
int toupper(int);
int strcmp(char *, u8 *);

static inline void Zip_Close(Zip *z) {
    Mem_Free(z->x4C);
    Mem_Free(z->x14);
    Mem_Free(z->xC);
    if (z->x4 != NULL) {
        RomFile_Close(z->x4);
    }
    Mem_Free(z->x0);
    Mem_Free(z);
}

s32 Zip_ReadFileByName(void *arg0, u8 *name, void **outBuf, u32 *outSize) {
    Zip *z;
    ZipEntry *e;
    char buf[12];
    u8 *p;
    u8 *q;
    u8 *path;
    void *data;
    s32 match;
    s32 c;

    z = Zip_Open(arg0);
    if (z == NULL) {
        goto end;
    }
    goto next;
top:
        sprintf(buf, D_800BBA5C, e->x10);
        path = e->x30;
        p = name;
        q = strrchr(path, '/');
        if (q == NULL) {
            q = path;
        } else {
            q++;
        }
        if ((c = *p) != 0) {
        cmp:
            if (toupper(c) == toupper(*q)) {
                p++;
                q++;
                if ((c = *p) != 0) goto cmp;
            }
        }
        match = (*p == 0 && *q == 0);
        if (match || strcmp(buf, name) == 0) {
            if ((*outBuf = Mem_AllocAligned(*outSize = e->x18, 7, 0)) == NULL) {
                goto fail;
            }
            if (func_80053050(z, e, *outBuf) != 0) {
                Mem_Free(*outBuf);
                goto fail;
            }
            Zip_Close(z);
            return 0;
        }
next:
    if ((e = func_80052AE0(z)) != NULL) {
        goto top;
    }
fail:
    Zip_Close(z);
end:
    return -1;
}
