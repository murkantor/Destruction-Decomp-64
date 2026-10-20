/*
 * Controller Pak: the libultra file system calls (osPfs*) over a 32 KB image
 * per controller, kept in Data/Saves/pak<n>.mpk in the real Controller Pak
 * layout, big-endian, so emulators and pak tools can open it.
 *
 * Layout (one bank of 128 pages of 256 bytes):
 *   page 0      label (block 0) and the pak ID, with backups at 0x60, 0x80, 0xC0
 *   pages 1, 2  the inode table and its copy: a 16-bit entry per page holding
 *               the file's next page, PFS_EOF or PFS_PAGE_FREE
 *   pages 3, 4  the directory: 16 notes of 32 bytes
 *   pages 5-127 note data
 *
 * Note data goes between game memory and the image in N64 byte order (see
 * Port_ReadN64Bytes), so a note saved here is the note the cartridge would save.
 */
#include <SDL3/SDL.h>
#include <string.h>

#include "port.h"
#include "port_config.h"
#include "port_shadow.h"

#define PAK_SIZE 0x8000
#define PAGE_SIZE 256
#define BLOCK_SIZE 32
#define PAGES 128
#define INODE_PAGE 1
#define INODE_BACKUP_PAGE 2
#define DIR_PAGE 3
#define DIR_ENTRIES 16
#define FIRST_DATA_PAGE 5

#define PFS_EOF 1
#define PFS_PAGE_FREE 3
#define DIR_STATUS_OCCUPIED 2

#define ID_OFFSET 0x20
static const u32 sIdBackups[] = { 0x60, 0x80, 0xC0 };

typedef struct {
    int loaded;
    int dirty;
    u8 mem[PAK_SIZE];
} Pak;

static Pak sPaks[MAXCONTROLLERS];

/* ---- The image ---- */

static u16 get16(const u8 *p) {
    return (u16)(p[0] << 8 | p[1]);
}

static u32 get32(const u8 *p) {
    return (u32)p[0] << 24 | (u32)p[1] << 16 | (u32)p[2] << 8 | p[3];
}

static void put16(u8 *p, u16 v) {
    p[0] = v >> 8;
    p[1] = (u8)v;
}

static void put32(u8 *p, u32 v) {
    p[0] = v >> 24;
    p[1] = (u8)(v >> 16);
    p[2] = (u8)(v >> 8);
    p[3] = (u8)v;
}

static void pakPath(char *buf, size_t size, int channel) {
    SDL_snprintf(buf, size, "%sData/Saves/pak%d.mpk", Port_BaseDir(), channel + 1);
}

/* The ID block's two checksums, over its first 28 bytes as 16-bit words */
static void idChecksum(u8 *id) {
    u16 sum = 0, inv = 0;
    int i;

    for (i = 0; i < 28; i += 2) {
        sum += get16(id + i);
        inv += (u16)~get16(id + i);
    }
    put16(id + 28, sum);
    put16(id + 30, inv);
}

static int idValid(const u8 *id) {
    u16 sum = 0, inv = 0;
    int i;

    for (i = 0; i < 28; i += 2) {
        sum += get16(id + i);
        inv += (u16)~get16(id + i);
    }
    return get16(id + 28) == sum && get16(id + 30) == inv && (get16(id + 24) & 1);
}

/* The inode table's checksum: the low byte of the sum of its data entries' bytes */
static void inodeChecksum(u8 *page) {
    u32 sum = 0;
    int i;

    for (i = FIRST_DATA_PAGE * 2; i < PAGE_SIZE; i++) {
        sum += page[i];
    }
    page[1] = (u8)sum;
}

static int inodeValid(const u8 *page) {
    u32 sum = 0;
    int i;

    for (i = FIRST_DATA_PAGE * 2; i < PAGE_SIZE; i++) {
        sum += page[i];
    }
    return page[1] == (u8)sum;
}

/* A freshly formatted pak, as libultra's repair and the pak tools make it */
static void format(u8 *m) {
    u8 id[32];
    u64 t = SDL_GetTicksNS() ^ (u64)SDL_GetPerformanceCounter();
    int i;

    memset(m, 0, PAK_SIZE);
    memset(id, 0, sizeof(id));
    put32(id + 0, 0xFFFFFFFF);       /* repaired */
    put32(id + 4, (u32)t);           /* random */
    put32(id + 8, (u32)(t >> 32));   /* serial number */
    put32(id + 12, 0x0A5A5A5A);
    put32(id + 16, (u32)(t * 2654435761u));
    put32(id + 20, 0x00000000);
    put16(id + 24, 0x0001);          /* device ID: bit 0 set on a working pak */
    id[26] = 1;                      /* banks */
    id[27] = 0;                      /* version */
    idChecksum(id);
    memcpy(m + ID_OFFSET, id, 32);
    for (i = 0; i < 3; i++) {
        memcpy(m + sIdBackups[i], id, 32);
    }
    for (i = FIRST_DATA_PAGE; i < PAGES; i++) {
        put16(m + INODE_PAGE * PAGE_SIZE + i * 2, PFS_PAGE_FREE);
    }
    inodeChecksum(m + INODE_PAGE * PAGE_SIZE);
    memcpy(m + INODE_BACKUP_PAGE * PAGE_SIZE, m + INODE_PAGE * PAGE_SIZE, PAGE_SIZE);
}

static void save(int channel) {
    Pak *pak = &sPaks[channel];
    char path[1024], tmp[1040];
    SDL_IOStream *io;

    if (!pak->dirty) {
        return;
    }
    SDL_snprintf(path, sizeof(path), "%sData/Saves", Port_BaseDir());
    SDL_CreateDirectory(path);
    pakPath(path, sizeof(path), channel);
    SDL_snprintf(tmp, sizeof(tmp), "%s.tmp", path);
    io = SDL_IOFromFile(tmp, "wb");
    if (io == NULL) {
        Port_Log("pak: cannot write %s: %s", tmp, SDL_GetError());
        return;
    }
    if (SDL_WriteIO(io, pak->mem, PAK_SIZE) != PAK_SIZE) {
        Port_Log("pak: writing %s failed: %s", tmp, SDL_GetError());
        SDL_CloseIO(io);
        return;
    }
    SDL_CloseIO(io);
    if (!SDL_RenamePath(tmp, path)) {
        Port_Log("pak: cannot replace %s: %s", path, SDL_GetError());
        return;
    }
    pak->dirty = 0;
}

/* The image for a channel, read from disk (or formatted) on first use */
static Pak *getPak(int channel) {
    Pak *pak;
    char path[1024];
    size_t size = 0;
    void *data;

    if (channel < 0 || channel >= MAXCONTROLLERS || !Port_PakPresent(channel)) {
        return NULL;
    }
    pak = &sPaks[channel];
    if (pak->loaded) {
        return pak;
    }
    pakPath(path, sizeof(path), channel);
    data = SDL_LoadFile(path, &size);
    if (data != NULL && size == PAK_SIZE) {
        memcpy(pak->mem, data, PAK_SIZE);
        Port_Log("pak: controller %d: %s", channel + 1, path);
    } else {
        if (data != NULL) {
            Port_Log("pak: %s is %u bytes, not %u: formatting a new pak instead", path, (unsigned)size,
                     (unsigned)PAK_SIZE);
            SDL_snprintf(path + SDL_strlen(path), sizeof(path) - SDL_strlen(path), ".bad");
            SDL_SaveFile(path, data, size);
        } else {
            Port_Log("pak: controller %d: new pak", channel + 1);
        }
        format(pak->mem);
        pak->dirty = 1;
    }
    SDL_free(data);
    pak->loaded = 1;
    if (!idValid(pak->mem + ID_OFFSET)) {
        /* libultra repairs the ID from a backup */
        int i;
        for (i = 0; i < 3 && !idValid(pak->mem + sIdBackups[i]); i++) {
        }
        if (i < 3) {
            memcpy(pak->mem + ID_OFFSET, pak->mem + sIdBackups[i], 32);
            pak->dirty = 1;
        }
    }
    save(channel);
    return pak;
}

static u8 *inodes(Pak *pak) {
    return pak->mem + INODE_PAGE * PAGE_SIZE;
}

static u8 *dirEntry(Pak *pak, int file) {
    return pak->mem + DIR_PAGE * PAGE_SIZE + file * 32;
}

static int dirUsed(const u8 *d) {
    return get32(d + 0) != 0 && get16(d + 4) != 0;
}

/* Rewrite the inode checksum and its copy after a change */
static void inodesChanged(Pak *pak) {
    inodeChecksum(inodes(pak));
    memcpy(pak->mem + INODE_BACKUP_PAGE * PAGE_SIZE, inodes(pak), PAGE_SIZE);
    pak->dirty = 1;
}

static int freePages(Pak *pak) {
    int n = 0, i;

    for (i = FIRST_DATA_PAGE; i < PAGES; i++) {
        n += get16(inodes(pak) + i * 2) == PFS_PAGE_FREE;
    }
    return n;
}

/* The pages of a note, in order; -1 if the chain is broken */
static int notePages(Pak *pak, const u8 *d, u8 *list) {
    int n = 0;
    u16 page = get16(d + 6);

    while (page != PFS_EOF) {
        if (page < FIRST_DATA_PAGE || page >= PAGES || n >= PAGES) {
            return -1;
        }
        list[n++] = (u8)page;
        page = get16(inodes(pak) + page * 2);
    }
    return n;
}

static void releasePages(Pak *pak, u16 page) {
    int n = 0;

    while (page >= FIRST_DATA_PAGE && page < PAGES && n++ < PAGES) {
        u16 next = get16(inodes(pak) + page * 2);
        put16(inodes(pak) + page * 2, PFS_PAGE_FREE);
        page = next;
    }
}

/* ---- Game memory ---- */

/* Names come from game memory in N64 byte order */
static void readName(u8 *out, const u8 *name, int len) {
    if (name == NULL) {
        memset(out, 0, len);
    } else {
        Port_ReadN64Bytes(out, name, len);
    }
}

/* Fields the host writes into game structs are host order */
static void hostWrote(const void *p, u32 len) {
    Port_MarkNative(p, len);
}

static Pak *pakFor(OSPfs *pfs) {
    if (pfs == NULL || !(pfs->status & PFS_INITIALIZED)) {
        return NULL;
    }
    return getPak(pfs->channel);
}

/* ---- libultra ---- */

int Port_PakPresent(int channel) {
    if (channel < 0 || channel >= MAXCONTROLLERS || !Port_InputConnected(channel)) {
        return 0;
    }
    return channel == 0 || gPortConfig.pakOnAllPorts;
}

s32 osPfsIsPlug(OSMesgQueue *mq, u8 *pattern) {
    u8 bits = 0;
    int i;

    for (i = 0; i < MAXCONTROLLERS; i++) {
        if (Port_PakPresent(i)) {
            bits |= 1 << i;
        }
    }
    *pattern = bits;
    hostWrote(pattern, 1);
    return 0;
}

s32 osPfsInitPak(OSMesgQueue *mq, OSPfs *pfs, int channel) {
    Pak *pak = getPak(channel);

    if (pak == NULL) {
        return PFS_ERR_NOPACK;
    }
    pfs->status = PFS_INITIALIZED;
    pfs->queue = mq;
    pfs->channel = channel;
    pfs->version = pak->mem[ID_OFFSET + 27];
    pfs->banks = 1;
    pfs->activebank = 0;
    pfs->dir_size = DIR_ENTRIES;
    pfs->inode_table = 1 * 8;
    pfs->minode_table = (1 + pfs->banks) * 8;
    pfs->dir_table = pfs->minode_table + pfs->banks * 8;
    pfs->inode_start_page = 1 + 2 + 2 * pfs->banks;
    hostWrote(pfs, sizeof(*pfs));
    Port_WriteN64Bytes(pfs->id, pak->mem + ID_OFFSET, 32);
    Port_WriteN64Bytes(pfs->label, pak->mem, 32);
    return 0;
}

/* Re-reads the pak ID (libultra's __osCheckPackId, after a change of pak) */
s32 func_8006FAA0(OSPfs *pfs) {
    if (!(pfs->status & 5)) {
        return PFS_ERR_INVALID;
    }
    if (getPak(pfs->channel) == NULL) {
        return PFS_ERR_NOPACK;
    }
    pfs->status &= ~4;
    hostWrote(&pfs->status, 4);
    return 0;
}

s32 osPfsChecker(OSPfs *pfs) {
    Pak *pak = pakFor(pfs);
    u8 used[PAGES], list[PAGES];
    int i, j, n, changed = 0;

    if (pak == NULL) {
        return pfs != NULL && (pfs->status & PFS_INITIALIZED) ? PFS_ERR_NOPACK : PFS_ERR_INVALID;
    }
    /* A broken table: take the copy if that one is good */
    if (!inodeValid(inodes(pak)) && inodeValid(pak->mem + INODE_BACKUP_PAGE * PAGE_SIZE)) {
        memcpy(inodes(pak), pak->mem + INODE_BACKUP_PAGE * PAGE_SIZE, PAGE_SIZE);
        changed = 1;
    }
    /* Drop notes whose page chains are broken or shared, then free pages no note uses */
    memset(used, 0, sizeof(used));
    for (i = 0; i < DIR_ENTRIES; i++) {
        u8 *d = dirEntry(pak, i);
        if (!dirUsed(d)) {
            continue;
        }
        n = notePages(pak, d, list);
        for (j = 0; j < n && !used[list[j]]; j++) {
        }
        if (n <= 0 || j < n) {
            Port_Log("pak: note %d is broken; deleted", i);
            memset(d, 0, 32);
            pak->dirty = 1;
            continue;
        }
        for (j = 0; j < n; j++) {
            used[list[j]] = 1;
        }
    }
    for (i = FIRST_DATA_PAGE; i < PAGES; i++) {
        if (!used[i] && get16(inodes(pak) + i * 2) != PFS_PAGE_FREE) {
            put16(inodes(pak) + i * 2, PFS_PAGE_FREE);
            changed = 1;
        }
    }
    if (changed || !inodeValid(inodes(pak))) {
        inodesChanged(pak);
    }
    save(pfs->channel);
    return 0;
}

s32 osPfsFreeBlocks(OSPfs *pfs, s32 *bytes) {
    Pak *pak = pakFor(pfs);

    if (pak == NULL) {
        return pfs != NULL && (pfs->status & PFS_INITIALIZED) ? PFS_ERR_NOPACK : PFS_ERR_INVALID;
    }
    *bytes = freePages(pak) * PAGE_SIZE;
    hostWrote(bytes, 4);
    return 0;
}

s32 osPfsFindFile(OSPfs *pfs, u16 company, u32 game, u8 *name, u8 *ext, s32 *file) {
    Pak *pak = pakFor(pfs);
    u8 n[16], e[4];
    int i;

    *file = -1;
    hostWrote(file, 4);
    if (pak == NULL) {
        return pfs != NULL && (pfs->status & PFS_INITIALIZED) ? PFS_ERR_NOPACK : PFS_ERR_INVALID;
    }
    readName(n, name, 16);
    readName(e, ext, 4);
    for (i = 0; i < DIR_ENTRIES; i++) {
        const u8 *d = dirEntry(pak, i);
        if (dirUsed(d) && get32(d) == game && get16(d + 4) == company && memcmp(d + 16, n, 16) == 0 &&
            memcmp(d + 12, e, 4) == 0) {
            *file = i;
            return 0;
        }
    }
    return PFS_ERR_INVALID;
}

s32 osPfsAllocateFile(OSPfs *pfs, u16 company, u32 game, u8 *name, u8 *ext, int length, s32 *file) {
    Pak *pak = pakFor(pfs);
    int pages, i, slot, prev;
    u8 n[16], e[4];
    s32 ret;
    u8 *d;

    if (company == 0 || game == 0) {
        return PFS_ERR_INVALID;
    }
    if (pak == NULL) {
        return pfs != NULL && (pfs->status & PFS_INITIALIZED) ? PFS_ERR_NOPACK : PFS_ERR_INVALID;
    }
    ret = osPfsFindFile(pfs, company, game, name, ext, file);
    if (ret != 0 && ret != PFS_ERR_INVALID) {
        return ret;
    }
    if (*file != -1) {
        return PFS_ERR_EXIST;
    }
    pages = (length + PAGE_SIZE - 1) / PAGE_SIZE;
    if (length > freePages(pak) * PAGE_SIZE) {
        return PFS_DATA_FULL;
    }
    if (pages <= 0) {
        return PFS_ERR_INVALID;
    }
    for (slot = 0; slot < DIR_ENTRIES && dirUsed(dirEntry(pak, slot)); slot++) {
    }
    if (slot == DIR_ENTRIES) {
        return PFS_DIR_FULL;
    }
    /* Chain the first free pages, in order */
    d = dirEntry(pak, slot);
    memset(d, 0, 32);
    prev = -1;
    for (i = FIRST_DATA_PAGE; i < PAGES && pages > 0; i++) {
        if (get16(inodes(pak) + i * 2) != PFS_PAGE_FREE) {
            continue;
        }
        if (prev < 0) {
            put16(d + 6, (u16)i);
        } else {
            put16(inodes(pak) + prev * 2, (u16)i);
        }
        put16(inodes(pak) + i * 2, PFS_EOF);
        memset(pak->mem + i * PAGE_SIZE, 0, PAGE_SIZE);
        prev = i;
        pages--;
    }
    inodesChanged(pak);
    readName(n, name, 16);
    readName(e, ext, 4);
    put32(d + 0, game);
    put16(d + 4, company);
    d[8] = DIR_STATUS_OCCUPIED;
    memcpy(d + 12, e, 4);
    memcpy(d + 16, n, 16);
    *file = slot;
    hostWrote(file, 4);
    save(pfs->channel);
    return 0;
}

s32 osPfsDeleteFile(OSPfs *pfs, u16 company, u32 game, u8 *name, u8 *ext) {
    Pak *pak = pakFor(pfs);
    s32 file, ret;
    u8 *d;

    if (company == 0 || game == 0) {
        return PFS_ERR_INVALID;
    }
    if (pak == NULL) {
        return pfs != NULL && (pfs->status & PFS_INITIALIZED) ? PFS_ERR_NOPACK : PFS_ERR_INVALID;
    }
    ret = osPfsFindFile(pfs, company, game, name, ext, &file);
    if (ret != 0) {
        return ret;
    }
    d = dirEntry(pak, file);
    releasePages(pak, get16(d + 6));
    memset(d, 0, 32);
    inodesChanged(pak);
    save(pfs->channel);
    return 0;
}

s32 osPfsReadWriteFile(OSPfs *pfs, s32 file, u8 flag, int offset, int size, u8 *data) {
    Pak *pak = pakFor(pfs);
    u8 list[PAGES];
    int n, done;
    u8 *d;

    if (file < 0 || file >= DIR_ENTRIES || size <= 0 || size % BLOCK_SIZE != 0 || offset < 0 ||
        offset % BLOCK_SIZE != 0) {
        return PFS_ERR_INVALID;
    }
    if (pak == NULL) {
        return pfs != NULL && (pfs->status & PFS_INITIALIZED) ? PFS_ERR_NOPACK : PFS_ERR_INVALID;
    }
    d = dirEntry(pak, file);
    if (!dirUsed(d)) {
        return PFS_ERR_INVALID;
    }
    n = notePages(pak, d, list);
    if (n < 0) {
        return PFS_ERR_INCONSISTENT;
    }
    if (offset + size > n * PAGE_SIZE) {
        return PFS_ERR_INVALID;
    }
    for (done = 0; done < size;) {
        int pos = offset + done;
        int chunk = PAGE_SIZE - pos % PAGE_SIZE;
        u8 *p = pak->mem + list[pos / PAGE_SIZE] * PAGE_SIZE + pos % PAGE_SIZE;
        if (chunk > size - done) {
            chunk = size - done;
        }
        if (flag == PFS_READ) {
            if (!Port_WriteN64Bytes(data + done, p, chunk)) {
                Port_Log("pak: note read into memory outside the RAM image (%p): left big-endian", data);
            }
        } else {
            if (!Port_ReadN64Bytes(p, data + done, chunk)) {
                Port_Log("pak: note written from memory outside the RAM image (%p): byte order unknown", data);
            }
        }
        done += chunk;
    }
    if (flag != PFS_READ) {
        pak->dirty = 1;
        save(pfs->channel);
    }
    return 0;
}

s32 osPfsFileState(OSPfs *pfs, s32 file, OSPfsState *state) {
    Pak *pak = pakFor(pfs);
    u8 list[PAGES];
    int n;
    u8 *d;

    if (file < 0 || file >= DIR_ENTRIES) {
        return PFS_ERR_INVALID;
    }
    if (pak == NULL) {
        return pfs != NULL && (pfs->status & PFS_INITIALIZED) ? PFS_ERR_NOPACK : PFS_ERR_INVALID;
    }
    d = dirEntry(pak, file);
    if (!dirUsed(d)) {
        return PFS_ERR_INVALID;
    }
    n = notePages(pak, d, list);
    if (n < 0) {
        return PFS_ERR_INCONSISTENT;
    }
    state->file_size = n * PAGE_SIZE;
    state->game_code = get32(d);
    state->company_code = get16(d + 4);
    hostWrote(state, 10);
    Port_WriteN64Bytes(state->ext_name, d + 12, 4);
    Port_WriteN64Bytes(state->game_name, d + 16, 16);
    return 0;
}
