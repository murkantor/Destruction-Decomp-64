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

#define Zip_Close(z)                 \
    {                                \
        Mem_Free((z)->x4C);          \
        Mem_Free((z)->x14);          \
        Mem_Free((z)->xC);           \
        if ((z)->x4 != NULL) {       \
            RomFile_Close((z)->x4);  \
        }                            \
        Mem_Free((z)->x0);           \
        Mem_Free(z);                 \
    }

s32 func_8005333C(char *path, u8 *name, s32 *outSize, s32 *outOffset) {
    Zip *z;
    ZipEntry *e;
    u8 *p;
    u8 *s;
    s32 match;
    u8 *fn;

    z = Zip_Open(path);
    if (z == NULL) {
        return -1;
    }
    goto next1;
loop1:
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
        goto found;
    }
next1:
    e = func_80052AE0(z);
    p = name;
    if (e != NULL) {
        goto loop1;
    }
    Zip_Close(z);
    z = Zip_Open(path);
    if (z == NULL) {
        return -1;
    }
    goto next2;
loop2:
    if (*outOffset != 0 && e->x10 == *outOffset) {
    found:
        *outSize = e->x18;
        *outOffset = e->x10;
        Zip_Close(z);
        return 0;
    }
next2:
    e = func_80052AE0(z);
    if (e != NULL) {
        goto loop2;
    }
    Zip_Close(z);
    return -1;
}
