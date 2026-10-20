/*
 * The user's Destruction Derby 64 (USA) cart image. It is looked for next to the
 * executable (Data/ first), accepted in any of the three common byte orders,
 * converted to the cart's own big-endian order and checked against the SHA1 of
 * the retail ROM. Nothing from it is ever written back out.
 */
#include <SDL3/SDL.h>
#include <string.h>

#include "port.h"

#define ROM_SIZE 0x1000000
#define ROM_SHA1 "a2c0799e13566d1b723299592cfcac9387f25fa7"

u8 *gPortRom;
u32 gPortRomSize;

/* ---- SHA1 (FIPS 180-1), only for checking the image ---- */

typedef struct {
    u32 h[5];
    u64 len;
    u8 buf[64];
    u32 used;
} Sha1;

#define ROL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

static void sha1Block(Sha1 *s, const u8 *p) {
    u32 w[80], a, b, c, d, e, t;
    int i;

    for (i = 0; i < 16; i++) {
        w[i] = (u32)p[i * 4] << 24 | (u32)p[i * 4 + 1] << 16 | (u32)p[i * 4 + 2] << 8 | p[i * 4 + 3];
    }
    for (; i < 80; i++) {
        w[i] = ROL(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
    }
    a = s->h[0], b = s->h[1], c = s->h[2], d = s->h[3], e = s->h[4];
    for (i = 0; i < 80; i++) {
        if (i < 20) {
            t = ((b & c) | (~b & d)) + 0x5A827999;
        } else if (i < 40) {
            t = (b ^ c ^ d) + 0x6ED9EBA1;
        } else if (i < 60) {
            t = ((b & c) | (b & d) | (c & d)) + 0x8F1BBCDC;
        } else {
            t = (b ^ c ^ d) + 0xCA62C1D6;
        }
        t += ROL(a, 5) + e + w[i];
        e = d, d = c, c = ROL(b, 30), b = a, a = t;
    }
    s->h[0] += a, s->h[1] += b, s->h[2] += c, s->h[3] += d, s->h[4] += e;
}

static void sha1(const u8 *data, u32 len, char out[41]) {
    Sha1 s = { { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 } };
    u8 tail[128];
    u32 rest = len % 64, pad, i;
    u64 bits = (u64)len * 8;

    for (i = 0; i + 64 <= len; i += 64) {
        sha1Block(&s, data + i);
    }
    memset(tail, 0, sizeof(tail));
    memcpy(tail, data + len - rest, rest);
    tail[rest] = 0x80;
    pad = rest < 56 ? 64 : 128;
    for (i = 0; i < 8; i++) {
        tail[pad - 1 - i] = (u8)(bits >> (i * 8));
    }
    sha1Block(&s, tail);
    if (pad == 128) {
        sha1Block(&s, tail + 64);
    }
    for (i = 0; i < 5; i++) {
        SDL_snprintf(out + i * 8, 9, "%08x", (unsigned)s.h[i]);
    }
}

/* ---- Loading ---- */

static int tryLoad(const char *path) {
    size_t size;
    u8 *data = SDL_LoadFile(path, &size);
    char digest[41];
    u32 i;

    if (data == NULL) {
        return 0;
    }
    if (size != ROM_SIZE) {
        Port_Log("%s: %u bytes, not a 16 MB cart image; skipped", path, (unsigned)size);
        SDL_free(data);
        return 0;
    }
    if (data[0] == 0x37 && data[1] == 0x80) { /* .v64: 16-bit words swapped */
        for (i = 0; i < size; i += 2) {
            u8 t = data[i];
            data[i] = data[i + 1];
            data[i + 1] = t;
        }
    } else if (data[0] == 0x40 && data[1] == 0x12) { /* .n64: 32-bit words reversed */
        for (i = 0; i < size; i += 4) {
            u8 t0 = data[i], t1 = data[i + 1];
            data[i] = data[i + 3];
            data[i + 1] = data[i + 2];
            data[i + 2] = t1;
            data[i + 3] = t0;
        }
    }
    if (data[0] != 0x80 || data[1] != 0x37) {
        Port_Log("%s: not an N64 cart image; skipped", path);
        SDL_free(data);
        return 0;
    }
    sha1(data, size, digest);
    if (strcmp(digest, ROM_SHA1) != 0) {
        Port_Log("%s: SHA1 %s is not Destruction Derby 64 (USA); skipped", path, digest);
        SDL_free(data);
        return 0;
    }
    gPortRom = data;
    gPortRomSize = size;
    Port_Log("Loaded %s", path);
    return 1;
}

static int tryDir(const char *dir) {
    static const char *const names[] = {
        "baserom.z64", "Destruction Derby 64 (USA).z64", "Destruction Derby 64 (USA).n64",
        "Destruction Derby 64 (USA).v64", "dd64.z64", "dd64.n64", "dd64.v64",
    };
    char path[1024];
    char **list;
    int count, i;

    for (i = 0; i < (int)SDL_arraysize(names); i++) {
        SDL_snprintf(path, sizeof(path), "%s%s", dir, names[i]);
        if (tryLoad(path)) {
            return 1;
        }
    }
    /* Any other 16 MB image with the right SHA1 */
    list = SDL_GlobDirectory(dir, "*.[zvn]64", SDL_GLOB_CASEINSENSITIVE, &count);
    if (list != NULL) {
        for (i = 0; i < count; i++) {
            SDL_snprintf(path, sizeof(path), "%s%s", dir, list[i]);
            if (tryLoad(path)) {
                SDL_free(list);
                return 1;
            }
        }
        SDL_free(list);
    }
    return 0;
}

int Port_RomLoad(void) {
    const char *base = SDL_GetBasePath();
    const char *env = SDL_getenv("DD64_ROM");
    char dir[1024];

    if (env != NULL && tryLoad(env)) {
        return 1;
    }
    SDL_snprintf(dir, sizeof(dir), "%sData/", base != NULL ? base : "");
    if (tryDir(dir)) {
        return 1;
    }
    SDL_snprintf(dir, sizeof(dir), "%s", base != NULL ? base : "");
    return tryDir(dir);
}

u32 Port_RomRead32(u32 offset) {
    const u8 *p = gPortRom + offset;

    if (offset + 4 > gPortRomSize) {
        return 0;
    }
    return (u32)p[0] << 24 | (u32)p[1] << 16 | (u32)p[2] << 8 | p[3];
}
