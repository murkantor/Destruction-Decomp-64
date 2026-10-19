#include "common.h"
#include "m2c.h"

typedef struct {
    void *x0;
    void *x4;
    u8 pad8[4];
    void *xC;
    u8 pad10[4];
    void *x14;
    u8 pad18[0x34];
    void *x4C;
} Zip;

typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[4];
    s32 x18;
    u8 pad1C[0x14];
    u8 *x30;
} ZipEntry;

Zip *Zip_Open(char *);
ZipEntry *func_80052AE0(Zip *);
void Mem_Free(void *);
s32 RomFile_Close(void *);
u8 *strrchr(const u8 *, int);
int toupper(int);

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

s32 Zip_FindEntry(char *path, u8 *name, s32 *outSize, s32 *outOffset) {
    Zip *z;
    ZipEntry *e;
    u8 *p;
    u8 *s;
    s32 match;
    u8 *fn;
    s32 c;

    z = Zip_Open(path);
    if (z != NULL) {
        goto next1;
        do {
        p = name;
        fn = e->x30;
        s = strrchr(fn, '/');
        if (s != NULL) {
            s++;
        } else {
            s = fn;
        }
        while (*p != 0 && toupper(*p) == toupper(*s)) {
            p++;
            s++;
        }
        match = (*p == 0 && *s == 0);
        if (match) {
            /* do/while (0) wrappers weight e's refs so it gets s2 */
            do *outSize = e->x18; while (0);
            do *outOffset = e->x10; while (0);
            Zip_Close(z);
            return 0;
        }
    next1:
        e = func_80052AE0(z);
        } while (e != NULL);
        Zip_Close(z);
        z = Zip_Open(path);
        if (z != NULL) {
            goto next2;
            do {
            if (*outOffset != 0 && e->x10 == *outOffset) {
                do *outSize = e->x18; while (0);
                do *outOffset = e->x10; while (0);
                Zip_Close(z);
                return 0;
            }
        next2:
            e = func_80052AE0(z);
            } while (e != NULL);
            Zip_Close(z);
        }
    }
    return -1;
}
