#include "common.h"
#include "m2c.h"

typedef struct {
    u32 signature;
    u8 version_made_by[2];
    u8 version_needed[2];
    u16 flags;
    u16 method;
    u16 time;
    u16 date;
    u32 crc;
    u32 csize;
    u32 ucsize;
    u16 name_len;
    u16 extra_len;
    u16 comment_len;
    u16 disk;
    u16 int_attr;
    u32 ext_attr;
    u32 offset;
    char *name;
} CdirEntry;

typedef struct {
    u8 pad0[0x14];
    u8 *buf;
    u32 pos;
    CdirEntry e;
    u8 pad50[0xC];
    u32 size;
} ZipDir;

void *Mem_AllocAlt(u32 size);
void Mem_Free(void *);
void *memcpy(void *, const void *, u32);

static inline u16 SH(u8 *p) {
    return (p[1] << 8) | p[0];
}

static inline u32 LG(u8 *p) {
    return (p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0];
}
#define P(z) ((z)->buf + (z)->pos)

CdirEntry *Zip_ReadNextEntry(ZipDir *z) {
    if (z->pos >= z->size) {
        return NULL;
    }
    z->e.signature = LG(P(z));
    z->e.version_made_by[0] = P(z)[4];
    z->e.version_made_by[1] = P(z)[5];
    z->e.version_needed[0] = P(z)[6];
    z->e.version_needed[1] = P(z)[7];
    z->e.flags = SH(P(z) + 8);
    z->e.method = SH(P(z) + 10);
    z->e.time = SH(P(z) + 12);
    z->e.date = SH(P(z) + 14);
    z->e.crc = LG(P(z) + 16);
    z->e.csize = LG(P(z) + 20);
    z->e.ucsize = LG(P(z) + 24);
    z->e.name_len = SH(P(z) + 28);
    z->e.extra_len = SH(P(z) + 30);
    z->e.comment_len = SH(P(z) + 32);
    z->e.disk = SH(P(z) + 34);
    z->e.int_attr = SH(P(z) + 36);
    z->e.ext_attr = LG(P(z) + 38);
    z->e.offset = LG(P(z) + 42);
    if (z->e.name != NULL) {
        Mem_Free(z->e.name);
    }
    z->e.name = Mem_AllocAlt(z->e.name_len + 1);
    memcpy(z->e.name, P(z) + 46, z->e.name_len);
    z->e.name[z->e.name_len] = 0;
    z->pos += 46 + z->e.name_len + z->e.extra_len + z->e.comment_len;
    return &z->e;
}
