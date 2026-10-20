/*
 * Audio: the RSP audio microcode (aspMain) run on the host, and the AI (the
 * DAC) as an SDL3 audio stream.
 *
 * The game's audio thread (the SDK audio manager, src/auto/68B68.c and
 * 68CA8.c) builds an audio command list each video field with the libultra
 * synthesizer and submits it as an SP task; the scheduler hands it to
 * Port_AudioRunTask (port/src/rcp.c), which interprets it here. The commands
 * work on a 4 KB scratch (the RSP's DMEM) and move samples in and out of game
 * memory: ADPCM data the game DMAs from the ROM, the reverb's delay line, the
 * decoder and envelope state blocks, and the output buffer. The thread then
 * hands the output buffer to the AI (osAiSetNextBuffer -> Port_AudioQueue).
 *
 * Game memory is read and written with the shadow map's helpers
 * (port/include/port_shadow.h): sample data and ADPCM books come from the ROM
 * (big-endian, marked), state blocks are only ever touched here (host order).
 * The buffers the interpreter writes for its own later reading (the delay
 * line) and for the AI are written in N64 byte order and marked big-endian,
 * so they read back the same either way.
 *
 * The command semantics follow mupen64plus-rsp-hle (alist.c, alist_audio.c:
 * the "audio" ucode variant, with the exponential envelope ramps of the
 * original libultra microcode), which is what emulators play this game with.
 *
 * Pacing: the audio manager sizes each frame from osAiGetLength, the bytes
 * left in the AI's current DMA, so that it stays about one field plus 96
 * samples ahead of the DAC (the formula overflows if the length ever exceeds
 * a field, which the hardware cannot report). Here the samples go into a ring
 * the SDL device pulls from in chunks, and the AI is modelled on top: each
 * buffer the game queues is a DMA, and the DAC's position is what the device
 * has taken plus a fixed latency of a few fields. The game then keeps the
 * ring at that latency, and its regulation (which absorbs the difference
 * between the host's field rate and the device's clock) is unchanged. The
 * device starts pulling once the ring has filled to the latency.
 *
 * Testing aids: DD64_WAVOUT=path writes the samples the game queued (before
 * the volume setting) to a WAV file; DD64_AUDIO_DUMP=path records every ADPCM
 * command of the first voice (state, book, input bytes, output; checked
 * against an independent decoder by tools/port/adpcm_check.py) and, in
 * path.sizes, a record per buffer queued (bytes, ms, ring fill, last length
 * reported), and logs the ADPCM book, the first frames and the frame-header
 * histograms; DD64_AUDIO_LATENCY=ms overrides the dd64.ini latency.
 */
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "port.h"
#include "port_config.h"
#include "port_shadow.h"

/* ---- The AI ---- */

#define RING_BYTES (256 * 1024) /* over a second at any rate the game could ask for */
#define MAX_DMAS 256

static SDL_Mutex *sLock;
static SDL_AudioStream *sStream;
static u32 sRate;              /* the rate the game set, Hz */
static u8 sRing[RING_BYTES];   /* s16 stereo, host order, at sRate */
static u32 sHead, sFill;       /* read position and bytes queued */
static u32 sLatencyBytes;      /* what the ring holds before the device plays: the DAC's lead over the device */
static int sPrimed;            /* the device is pulling from the ring */
static u64 sQueued;            /* bytes the game queued, in all */
static u64 sTaken;             /* bytes the device took from the ring, in all */
static u64 sTakenNs;           /* when it last did */
static struct {
    u64 start; /* in the stream of bytes queued */
    u32 size;
} sDmas[MAX_DMAS];             /* the buffers the game queued (the AI's DMAs), oldest first */
static u32 sDmaHead, sDmaCount;
static u32 sLastReport;        /* what osAiGetLength last returned */
static u32 sUnderruns;
static u64 sFirstQueueNs;
static int sQueueLogs;
static FILE *sWav;
static u32 sWavBytes;
static FILE *sDump, *sSizes;

static void SDLCALL pullFromRing(void *userdata, SDL_AudioStream *stream, int additional, int total) {
    u8 buf[4096];
    u32 want = (u32)(additional > 0 ? additional : 0) & ~3u, have, vol;

    if (want == 0) {
        return;
    }
    SDL_LockMutex(sLock);
    if (!sPrimed && sFill >= sLatencyBytes) {
        sPrimed = 1;
    }
    have = sPrimed ? SDL_min(want, sFill) : 0;
    if (sPrimed && have < want) {
        /* A gap of what is missing; the game's regulation refills the ring over the next fields */
        sUnderruns++;
        if (sUnderruns <= 5 || sUnderruns % 100 == 0) {
            Port_Log("audio: underrun %u: the device asked for %u bytes, %u in the ring, %llu queued in all",
                     (unsigned)sUnderruns, (unsigned)want, (unsigned)sFill, (unsigned long long)sQueued);
        }
    }
    vol = (u32)SDL_clamp(gPortConfig.volume, 0, 100) * 32768u / 100u;
    sTakenNs = SDL_GetTicksNS();
    while (have > 0) {
        u32 n = SDL_min(have, sizeof(buf)), first = SDL_min(n, RING_BYTES - sHead), i;

        memcpy(buf, sRing + sHead, first);
        memcpy(buf + first, sRing, n - first);
        sHead = (sHead + n) % RING_BYTES;
        sFill -= n;
        sTaken += n;
        have -= n;
        want -= n;
        if (vol != 32768u) {
            for (i = 0; i < n; i += 2) {
                s16 s;
                memcpy(&s, buf + i, 2);
                s = (s16)(((s32)s * (s32)vol) >> 15);
                memcpy(buf + i, &s, 2);
            }
        }
        SDL_PutAudioStreamData(stream, buf, (int)n);
    }
    SDL_UnlockMutex(sLock);
    memset(buf, 0, sizeof(buf));
    while (want > 0) {
        u32 n = SDL_min(want, sizeof(buf));
        SDL_PutAudioStreamData(stream, buf, (int)n);
        want -= n;
    }
}

static void wavFinish(void) {
    u32 v;

    if (sWav == NULL) {
        return;
    }
    v = 36 + sWavBytes;
    fseek(sWav, 4, SEEK_SET);
    fwrite(&v, 4, 1, sWav);
    v = sRate;
    fseek(sWav, 24, SEEK_SET);
    fwrite(&v, 4, 1, sWav);
    v = sRate * 4;
    fwrite(&v, 4, 1, sWav);
    v = sWavBytes;
    fseek(sWav, 40, SEEK_SET);
    fwrite(&v, 4, 1, sWav);
    fclose(sWav);
    sWav = NULL;
}

static void dumpStats(void);

static void audioAtExit(void) {
    u64 now = SDL_GetTicksNS();

    dumpStats();
    if (sFirstQueueNs != 0 && now > sFirstQueueNs) {
        double secs = (double)(now - sFirstQueueNs) / 1e9;
        Port_Log("audio: %llu frames queued in %.2f s = %.1f Hz (rate %u Hz), %u underruns, ring %u bytes",
                 (unsigned long long)(sQueued / 4), secs, (double)(sQueued / 4) / secs, (unsigned)sRate,
                 (unsigned)sUnderruns, (unsigned)sFill);
    }
    wavFinish();
}

void Port_AudioInit(void) {
    const char *wav = SDL_getenv("DD64_WAVOUT");

    sLock = SDL_CreateMutex();
    /* Smaller device chunks, so the ring is drained evenly (a hint; the backend may ignore it) */
    SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES, "512");
    if (wav != NULL) {
        sWav = fopen(wav, "wb");
        if (sWav != NULL) {
            static const u8 header[44] = { 'R', 'I', 'F', 'F', 0, 0, 0, 0, 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ',
                                           16, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 16, 0,
                                           'd', 'a', 't', 'a', 0, 0, 0, 0 };
            fwrite(header, sizeof(header), 1, sWav);
        } else {
            Port_Log("audio: cannot write %s", wav);
        }
    }
    if (SDL_getenv("DD64_AUDIO_DUMP") != NULL) {
        char path[1024];
        sDump = fopen(SDL_getenv("DD64_AUDIO_DUMP"), "wb");
        SDL_snprintf(path, sizeof(path), "%s.sizes", SDL_getenv("DD64_AUDIO_DUMP")); /* bytes of each buffer queued, u32 */
        sSizes = fopen(path, "wb");
    }
    atexit(audioAtExit);
}

void Port_AudioSetFrequency(u32 freq) {
    SDL_AudioSpec spec;
    const char *lat = SDL_getenv("DD64_AUDIO_LATENCY");
    u32 ms = lat != NULL ? (u32)SDL_strtoul(lat, NULL, 10) : (u32)SDL_clamp(gPortConfig.audioLatency, 20, 1000);

    SDL_AudioStream *old, *stream;

    /* The device thread holds SDL's device lock while pullFromRing waits for sLock,
       so the stream is closed and opened outside sLock */
    SDL_LockMutex(sLock);
    if (sStream != NULL && sRate == freq) {
        SDL_UnlockMutex(sLock);
        return;
    }
    old = sStream;
    sStream = NULL;
    sRate = freq;
    sHead = sFill = 0;
    sPrimed = 0;
    sQueued = sTaken = 0;
    sDmaHead = sDmaCount = 0;
    if (sWav != NULL) { /* the rate now, the sizes at exit (a crash leaves the sizes 0) */
        u32 v = freq;
        fseek(sWav, 24, SEEK_SET);
        fwrite(&v, 4, 1, sWav);
        v = freq * 4;
        fwrite(&v, 4, 1, sWav);
        fseek(sWav, 0, SEEK_END);
    }
    sLatencyBytes = (ms * freq / 1000) * 4;
    SDL_UnlockMutex(sLock);

    if (old != NULL) {
        SDL_DestroyAudioStream(old);
    }
    spec.format = SDL_AUDIO_S16;
    spec.channels = 2;
    spec.freq = (int)freq;
    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, pullFromRing, NULL);
    if (stream == NULL) {
        Port_Log("audio: no output device (%s); running silent", SDL_GetError());
        return;
    }
    {
        SDL_AudioSpec dev;
        int frames = 0;

        SDL_GetAudioDeviceFormat(SDL_GetAudioStreamDevice(stream), &dev, &frames);
        Port_Log("audio: %u Hz, latency %u samples (%u ms); device %d Hz, %d channels, %d-frame chunks",
                 (unsigned)freq, (unsigned)sLatencyBytes / 4, (unsigned)(sLatencyBytes / 4 * 1000 / freq), dev.freq,
                 dev.channels, frames);
    }
    SDL_LockMutex(sLock);
    sStream = stream;
    SDL_UnlockMutex(sLock);
    SDL_ResumeAudioStreamDevice(stream);
}

/* The game hands a buffer of s16 stereo samples (N64 byte order in RAM) to the AI. */
void Port_AudioQueue(const void *buf, u32 bytes) {
    const u8 *p = buf;
    u32 i;

    bytes &= ~3u;
    SDL_LockMutex(sLock);
    if (sFirstQueueNs == 0) {
        sFirstQueueNs = SDL_GetTicksNS();
    }
    if (sQueueLogs < 3) {
        sQueueLogs++;
        Port_Log("audio: queue %u bytes (%u samples), %u bytes in the ring", (unsigned)bytes, (unsigned)bytes / 4,
                 (unsigned)sFill);
    }
    if (bytes > RING_BYTES - sFill) {
        /* The device is not keeping up (or there is none): drop the oldest, as if played */
        u32 drop = bytes - (RING_BYTES - sFill);
        sHead = (sHead + drop) % RING_BYTES;
        sFill -= drop;
        sTaken += drop;
    }
    for (i = 0; i < bytes; i += 2) {
        u16 v = (u16)Port_RdN64(p + i, 2);
        memcpy(sRing + (sHead + sFill + i) % RING_BYTES, &v, 2);
        if (sWav != NULL) {
            fwrite(&v, 2, 1, sWav);
        }
    }
    if (sSizes != NULL) {
        u32 rec[4] = { bytes, (u32)SDL_GetTicks(), sFill, sLastReport };
        fwrite(rec, sizeof(rec), 1, sSizes);
    }
    sFill += bytes;
    if (sDmaCount == MAX_DMAS) {
        sDmaHead = (sDmaHead + 1) % MAX_DMAS;
        sDmaCount--;
    }
    sDmas[(sDmaHead + sDmaCount) % MAX_DMAS].start = sQueued;
    sDmas[(sDmaHead + sDmaCount) % MAX_DMAS].size = bytes;
    sDmaCount++;
    sQueued += bytes;
    sWavBytes += sWav != NULL ? bytes : 0;
    SDL_UnlockMutex(sLock);
}

/*
 * Bytes left in the AI's current DMA, as the game sees it (osAiGetLength). The
 * DAC is the latency ahead of what the device has taken, and runs on between
 * the device's pulls; it is in the oldest buffer it has not finished.
 */
u32 Port_AudioQueuedBytes(void) {
    u64 pos;
    u32 n = 0;

    SDL_LockMutex(sLock);
    pos = sTaken + sLatencyBytes;
    if (sPrimed) {
        u64 now = SDL_GetTicksNS();
        if (now > sTakenNs) {
            pos += ((now - sTakenNs) * sRate / 1000000000ull) * 4;
        }
    }
    pos = SDL_min(pos, sQueued);
    while (sDmaCount > 0 && sDmas[sDmaHead].start + sDmas[sDmaHead].size <= pos) {
        sDmaHead = (sDmaHead + 1) % MAX_DMAS;
        sDmaCount--;
    }
    if (sDmaCount > 0) {
        u64 end = sDmas[sDmaHead].start + sDmas[sDmaHead].size;
        n = pos > sDmas[sDmaHead].start ? (u32)(end - pos) : sDmas[sDmaHead].size;
    }
    sLastReport = n;
    SDL_UnlockMutex(sLock);
    return n;
}

/* ---- The audio microcode (aspMain) ---- */

#define A_SPNOOP 0
#define A_ADPCM 1
#define A_CLEARBUFF 2
#define A_ENVMIXER 3
#define A_LOADBUFF 4
#define A_RESAMPLE 5
#define A_SAVEBUFF 6
#define A_SEGMENT 7
#define A_SETBUFF 8
#define A_SETVOL 9
#define A_DMEMMOVE 10
#define A_LOADADPCM 11
#define A_MIXER 12
#define A_INTERLEAVE 13
#define A_POLEF 14
#define A_SETLOOP 15

#define A_INIT 0x01
#define A_LOOP 0x02
#define A_LEFT 0x02
#define A_VOL 0x04
#define A_AUX 0x08

#define DMEM_SIZE 0x1000

static s16 sDmem[DMEM_SIZE / 2];  /* the RSP's data memory, as 16-bit samples in host order */
static s16 sTable[0x100];         /* the ADPCM book (or the pole filter's coefficients) loaded */
static void *sLoop;               /* ADPCM loop state in game memory */
static u16 sIn, sOut, sCount;     /* A_SETBUFF */
static u16 sDryR, sWetL, sWetR;   /* A_SETBUFF | A_AUX */
static s16 sVol[2], sTarget[2];   /* A_SETVOL */
static s32 sRate2[2];
static s16 sDry, sWet;

/* The resampler's 4-tap windowed-sinc table, 64 phases (from the microcode's data) */
static const s16 sResampleLut[64 * 4] = {
    0x0c39, 0x66ad, 0x0d46, -0x0021, 0x0b39, 0x6696, 0x0e5f, -0x0028, 0x0a44, 0x6669, 0x0f83, -0x0030,
    0x095a, 0x6626, 0x10b4, -0x0038, 0x087d, 0x65cd, 0x11f0, -0x0041, 0x07ab, 0x655e, 0x1338, -0x004a,
    0x06e4, 0x64d9, 0x148c, -0x0054, 0x0628, 0x643f, 0x15eb, -0x005f, 0x0577, 0x638f, 0x1756, -0x006a,
    0x04d1, 0x62cb, 0x18cb, -0x0076, 0x0435, 0x61f3, 0x1a4c, -0x0082, 0x03a4, 0x6106, 0x1bd7, -0x008f,
    0x031c, 0x6007, 0x1d6c, -0x009c, 0x029f, 0x5ef5, 0x1f0b, -0x00aa, 0x022a, 0x5dd0, 0x20b3, -0x00b8,
    0x01be, 0x5c9a, 0x2264, -0x00c6, 0x015b, 0x5b53, 0x241e, -0x00d4, 0x0101, 0x59fc, 0x25e0, -0x00e2,
    0x00ae, 0x5896, 0x27a9, -0x00f0, 0x0063, 0x5720, 0x297a, -0x00fe, 0x001f, 0x559d, 0x2b50, -0x010c,
    -0x001e, 0x540d, 0x2d2c, -0x0118, -0x0054, 0x5270, 0x2f0d, -0x0125, -0x0084, 0x50c7, 0x30f3, -0x0130,
    -0x00ad, 0x4f14, 0x32dc, -0x013a, -0x00d2, 0x4d57, 0x34c8, -0x0143, -0x00f1, 0x4b91, 0x36b6, -0x014a,
    -0x010b, 0x49c2, 0x38a5, -0x0150, -0x0121, 0x47ed, 0x3a95, -0x0154, -0x0132, 0x4611, 0x3c85, -0x0155,
    -0x0140, 0x4430, 0x3e74, -0x0154, -0x014a, 0x424a, 0x4060, -0x0151, -0x0151, 0x4060, 0x424a, -0x014a,
    -0x0154, 0x3e74, 0x4430, -0x0140, -0x0155, 0x3c85, 0x4611, -0x0132, -0x0154, 0x3a95, 0x47ed, -0x0121,
    -0x0150, 0x38a5, 0x49c2, -0x010b, -0x014a, 0x36b6, 0x4b91, -0x00f1, -0x0143, 0x34c8, 0x4d57, -0x00d2,
    -0x013a, 0x32dc, 0x4f14, -0x00ad, -0x0130, 0x30f3, 0x50c7, -0x0084, -0x0125, 0x2f0d, 0x5270, -0x0054,
    -0x0118, 0x2d2c, 0x540d, -0x001e, -0x010c, 0x2b50, 0x559d, 0x001f, -0x00fe, 0x297a, 0x5720, 0x0063,
    -0x00f0, 0x27a9, 0x5896, 0x00ae, -0x00e2, 0x25e0, 0x59fc, 0x0101, -0x00d4, 0x241e, 0x5b53, 0x015b,
    -0x00c6, 0x2264, 0x5c9a, 0x01be, -0x00b8, 0x20b3, 0x5dd0, 0x022a, -0x00aa, 0x1f0b, 0x5ef5, 0x029f,
    -0x009c, 0x1d6c, 0x6007, 0x031c, -0x008f, 0x1bd7, 0x6106, 0x03a4, -0x0082, 0x1a4c, 0x61f3, 0x0435,
    -0x0076, 0x18cb, 0x62cb, 0x04d1, -0x006a, 0x1756, 0x638f, 0x0577, -0x005f, 0x15eb, 0x643f, 0x0628,
    -0x0054, 0x148c, 0x64d9, 0x06e4, -0x004a, 0x1338, 0x655e, 0x07ab, -0x0041, 0x11f0, 0x65cd, 0x087d,
    -0x0038, 0x10b4, 0x6626, 0x095a, -0x0030, 0x0f83, 0x6669, 0x0a44, -0x0028, 0x0e5f, 0x6696, 0x0b39,
    -0x0021, 0x0d46, 0x66ad, 0x0c39,
};

static inline s16 *dmem16(u32 addr) {
    return &sDmem[(addr >> 1) & (DMEM_SIZE / 2 - 1)];
}

static inline u8 *dmem8(u32 addr) {
    return &((u8 *)sDmem)[(addr & (DMEM_SIZE - 1)) ^ 1]; /* bytes as the big-endian RSP sees them */
}

static inline s16 clamp16(s32 x) {
    return (s16)(x < -32768 ? -32768 : x > 32767 ? 32767 : x);
}

static inline u32 align(u32 x, u32 n) {
    return (x + n - 1) & ~(n - 1);
}

/* A physical address from a command as a host pointer, or NULL if it is not game memory */
static void *ramPtr(u32 addr, u32 len, const char *what) {
    static int sReported;
    u8 *p;

    if (addr < 0x80000000u) {
        addr |= 0x80000000u; /* K0_TO_PHYS'd by the game (the ADPCM state, loop and book pointers) */
    }
    p = (u8 *)addr;
    if ((u32)(p - gRam) >= PORT_RAM_SIZE || len > PORT_RAM_SIZE - (u32)(p - gRam)) {
        if (sReported++ < 8) {
            Port_Log("audio: %s at %08X (%u bytes) is outside the game's memory", what, (unsigned)addr, (unsigned)len);
        }
        return NULL;
    }
    return p;
}

/* Samples in game memory: the game's data (big-endian, or converted) and the state we wrote (host order) */
static inline s16 rd16(const void *p) {
    return (s16)Port_RdN64(p, 2);
}

static void loadSamples(s16 *dst, const void *src, u32 n) {
    u32 i;

    for (i = 0; i < n; i++) {
        dst[i] = rd16((const u8 *)src + 2 * i);
    }
}

/*
 * The microcode's own state blocks: host order, read back only here and the
 * same way (plain copies, never the width-aware readers: GCC inlines these
 * copies, so the shadow map's marks on the block do not follow them).
 */
static void storeState(void *dst, const void *src, u32 bytes) {
    memcpy(dst, src, bytes);
}

/* DMEM <-> game memory in N64 byte order, so ADPCM bytes and samples alike go through unchanged */
static void loadBuffer(u32 dmem, const void *src, u32 bytes) {
    u8 tmp[DMEM_SIZE];
    u32 i;

    bytes = SDL_min(bytes, DMEM_SIZE - (dmem & (DMEM_SIZE - 1)));
    Port_ReadN64Bytes(tmp, src, bytes);
    for (i = 0; i < bytes; i++) {
        *dmem8(dmem + i) = tmp[i];
    }
}

static void saveBuffer(void *dst, u32 dmem, u32 bytes) {
    u8 tmp[DMEM_SIZE];
    u32 i;

    bytes = SDL_min(bytes, DMEM_SIZE - (dmem & (DMEM_SIZE - 1)));
    for (i = 0; i < bytes; i++) {
        tmp[i] = *dmem8(dmem + i);
    }
    Port_WriteN64Bytes(dst, tmp, bytes);
}

static s32 rdot(u32 n, const s16 *x, const s16 *y) {
    s32 accu = 0;

    y += n;
    while (n != 0) {
        accu += *(x++) * *(--y);
        --n;
    }
    return accu;
}

static void adpcmResiduals(s16 *dst, const s16 *src, const s16 *book, const s16 *last, u32 count) {
    const s16 *book1 = book, *book2 = book + 8;
    s16 l1 = last[0], l2 = last[1];
    u32 i;

    for (i = 0; i < count; i++) {
        s32 accu = (s32)src[i] << 11;
        accu += book1[i] * l1 + book2[i] * l2 + rdot(i, book2, src);
        dst[i] = clamp16(accu >> 11);
    }
}

/* DD64_AUDIO_DUMP=path: what the first ADPCM voice decodes, raw s16 mono, to check the decoder offline */
static u32 sDumpVoice, sDumpSamples, sDumpFrames;
static u32 sDumpScales[16], sDumpPreds[16];

static void dumpStats(void) {
    u32 i;
    char line[256], *p = line;

    if (sDumpFrames == 0) {
        return;
    }
    for (i = 0; i < 16; i++) {
        p += SDL_snprintf(p, line + sizeof(line) - p, " %u", (unsigned)sDumpScales[i]);
    }
    Port_Log("audio: %u adpcm frames; scale histogram:%s", (unsigned)sDumpFrames, line);
    p = line;
    for (i = 0; i < 16; i++) {
        p += SDL_snprintf(p, line + sizeof(line) - p, " %u", (unsigned)sDumpPreds[i]);
    }
    Port_Log("audio: predictor histogram:%s", line);
}

/* One record per ADPCM command of the first voice: what went in and what came out, for an independent re-decode */
static void audioDump(u32 stateAddr, u32 flags, const s16 *stateIn, u32 in, u32 count, const s16 *samples) {
    u32 hdr[4] = { 0xADC0ADC0u, flags, count, 0 };
    u32 i, n = count >> 1;

    if (sDump == NULL) {
        return;
    }
    if (sDumpVoice == 0) {
        sDumpVoice = stateAddr;
    }
    if (stateAddr != sDumpVoice || sDumpSamples >= 400000) {
        return;
    }
    fwrite(hdr, sizeof(hdr), 1, sDump);
    fwrite(stateIn, 2, 16, sDump);
    fwrite(sTable, 2, 128, sDump);
    for (i = 0; i < (count / 32) * 9; i++) {
        fputc(*dmem8(in + i), sDump);
    }
    fwrite(samples, 2, n, sDump);
    sDumpSamples += n;
}

static void cmdAdpcm(u32 flags, u32 addr) {
    void *state = ramPtr(addr, 32, "ADPCM state");
    s16 last[16], stateIn[16];
    u32 count = align(sCount, 32), in = sIn, out = sOut, i;

    if (state == NULL) {
        return;
    }
    if (flags & A_INIT) {
        memset(last, 0, sizeof(last));
    } else if (flags & A_LOOP) {
        if (sLoop == NULL) {
            return;
        }
        loadSamples(last, sLoop, 16); /* the bank's loop state: big-endian */
    } else {
        memcpy(last, state, sizeof(last)); /* our own, host order: read as written (the shadow map knows nothing of it) */
    }
    memcpy(stateIn, last, sizeof(stateIn));
    for (i = 0; i < 16; i++, out += 2) {
        *dmem16(out) = last[i];
    }
    while (count != 0) {
        s16 frame[16];
        u8 code = *dmem8(in++);
        u32 scale = code >> 4, rshift = scale < 12 ? 12 - scale : 0;
        const s16 *book = &sTable[(code & 0xF) << 4];

        if (sDump != NULL) {
            sDumpScales[scale]++;
            sDumpPreds[code & 0xF]++;
            if (sDumpFrames++ < 4) {
                Port_Log("audio: adpcm frame %u: in dmem %04X code %02X bytes %02X %02X %02X %02X book %04X %04X %04X %04X / %04X %04X %04X %04X",
                         sDumpFrames, (unsigned)(in - 1), code, *dmem8(in), *dmem8(in + 1), *dmem8(in + 2), *dmem8(in + 3),
                         (u16)book[0], (u16)book[1], (u16)book[2], (u16)book[3], (u16)book[8], (u16)book[9], (u16)book[10], (u16)book[11]);
            }
        }

        for (i = 0; i < 8; i++) {
            u8 b = *dmem8(in++);
            frame[2 * i] = (s16)((s16)((b & 0xF0) << 8) >> rshift);
            frame[2 * i + 1] = (s16)((s16)((b & 0x0F) << 12) >> rshift);
        }
        adpcmResiduals(last, frame, book, last + 14, 8);
        adpcmResiduals(last + 8, frame + 8, book, last + 6, 8);
        for (i = 0; i < 16; i++, out += 2) {
            *dmem16(out) = last[i];
        }
        count -= 32;
    }
    storeState(state, last, sizeof(last));
    audioDump(addr, flags, stateIn, sIn, align(sCount, 32), dmem16(sOut + 32));
}

static void cmdResample(u32 flags, u32 pitch, u32 addr) {
    void *state = ramPtr(addr, 16, "resampler state");
    u32 count = sCount, accu;
    u32 ipos = (sIn >> 1) - 4, opos = sOut >> 1;
    s16 saved[5];
    u32 i;

    if (state == NULL) {
        return;
    }
    count = align(count, 16) >> 1;
    pitch <<= 1;
    if (flags & A_INIT) {
        for (i = 0; i < 4; i++) {
            *dmem16(2 * (ipos + i)) = 0;
        }
        accu = 0;
    } else {
        memcpy(saved, state, sizeof(saved));
        for (i = 0; i < 4; i++) {
            *dmem16(2 * (ipos + i)) = saved[i];
        }
        accu = (u16)saved[4];
    }
    while (count != 0) {
        const s16 *lut = sResampleLut + ((accu & 0xFC00) >> 8);

        *dmem16(2 * opos++) = clamp16((*dmem16(2 * ipos) * lut[0] + *dmem16(2 * (ipos + 1)) * lut[1] +
                                       *dmem16(2 * (ipos + 2)) * lut[2] + *dmem16(2 * (ipos + 3)) * lut[3]) >>
                                      15);
        accu += pitch;
        ipos += accu >> 16;
        accu &= 0xFFFF;
        --count;
    }
    for (i = 0; i < 4; i++) {
        saved[i] = *dmem16(2 * (ipos + i));
    }
    saved[4] = (s16)accu;
    storeState(state, saved, sizeof(saved));
}

typedef struct {
    s64 value, step, target;
} Ramp;

static s16 rampStep(Ramp *r) {
    int reached;

    r->value += r->step;
    reached = r->step <= 0 ? r->value <= r->target : r->value >= r->target;
    if (reached) {
        r->value = r->target;
        r->step = 0;
    }
    return (s16)(r->value >> 16);
}

/* Exponential volume ramps, as the original microcode (alist_envmix_exp) */
static void cmdEnvMixer(u32 flags, u32 addr) {
    void *state = ramPtr(addr, 80, "envelope state");
    s16 save[40];
    Ramp ramps[2];
    s32 expSeq[2], expRates[2];
    s16 dry = sDry, wet = sWet;
    u32 n = (flags & A_AUX) ? 4 : 2, y, x, ptr = 0, j;
    u16 dmems[4];

    if (state == NULL) {
        return;
    }
    dmems[0] = sOut;
    dmems[1] = sDryR;
    dmems[2] = sWetL;
    dmems[3] = sWetR;
    memcpy(save, state, sizeof(save));
    if (flags & A_INIT) {
        ramps[0].value = (s64)sVol[0] << 16;
        ramps[1].value = (s64)sVol[1] << 16;
        ramps[0].target = (s64)sTarget[0] << 16;
        ramps[1].target = (s64)sTarget[1] << 16;
        expRates[0] = sRate2[0];
        expRates[1] = sRate2[1];
        expSeq[0] = sVol[0] * sRate2[0];
        expSeq[1] = sVol[1] * sRate2[1];
    } else {
        s32 v;
        wet = save[0];
        dry = save[2];
        memcpy(&v, save + 4, 4), ramps[0].target = v;
        memcpy(&v, save + 6, 4), ramps[1].target = v;
        memcpy(&expRates[0], save + 8, 4);
        memcpy(&expRates[1], save + 10, 4);
        memcpy(&expSeq[0], save + 12, 4);
        memcpy(&expSeq[1], save + 14, 4);
        memcpy(&v, save + 16, 4), ramps[0].value = v;
        memcpy(&v, save + 18, 4), ramps[1].value = v;
    }
    ramps[0].step = ramps[0].target - ramps[0].value;
    ramps[1].step = ramps[1].target - ramps[1].value;

    for (y = 0; y < sCount; y += 16) {
        for (j = 0; j < 2; j++) {
            if (ramps[j].step != 0) {
                expSeq[j] = (s32)(((s64)expSeq[j] * (s64)expRates[j]) >> 16);
                ramps[j].step = (expSeq[j] - ramps[j].value) >> 3;
            }
        }
        for (x = 0; x < 8; x++) {
            s16 lvol = rampStep(&ramps[0]), rvol = rampStep(&ramps[1]);
            s16 gains[4], in = *dmem16(sIn + 2 * ptr);

            gains[0] = clamp16((lvol * dry + 0x4000) >> 15);
            gains[1] = clamp16((rvol * dry + 0x4000) >> 15);
            gains[2] = clamp16((lvol * wet + 0x4000) >> 15);
            gains[3] = clamp16((rvol * wet + 0x4000) >> 15);
            for (j = 0; j < n; j++) {
                s16 *d = dmem16(dmems[j] + 2 * ptr);
                *d = clamp16(*d + ((in * gains[j]) >> 15));
            }
            ptr++;
        }
    }

    {
        s32 v;
        save[0] = wet;
        save[2] = dry;
        v = (s32)ramps[0].target, memcpy(save + 4, &v, 4);
        v = (s32)ramps[1].target, memcpy(save + 6, &v, 4);
        memcpy(save + 8, &expRates[0], 4);
        memcpy(save + 10, &expRates[1], 4);
        memcpy(save + 12, &expSeq[0], 4);
        memcpy(save + 14, &expSeq[1], 4);
        v = (s32)ramps[0].value, memcpy(save + 16, &v, 4);
        v = (s32)ramps[1].value, memcpy(save + 18, &v, 4);
    }
    storeState(state, save, sizeof(save));
}

static void cmdMixer(s16 gain, u32 dmemi, u32 dmemo) {
    u32 count = align(sCount, 32) >> 1, i;

    for (i = 0; i < count; i++) {
        s16 *d = dmem16(dmemo + 2 * i);
        *d = clamp16(*d + ((*dmem16(dmemi + 2 * i) * gain) >> 15));
    }
}

static void cmdInterleave(u32 left, u32 right) {
    u32 count = align(sCount, 16) >> 1, i;

    for (i = 0; i < count; i++) {
        *dmem16(sOut + 4 * i) = *dmem16(left + 2 * i);
        *dmem16(sOut + 4 * i + 2) = *dmem16(right + 2 * i);
    }
}

/* The reverb's low-pass: a pole filter over 8-sample frames, its coefficients loaded by A_LOADADPCM */
static void cmdPoleFilter(u32 flags, u16 gain, u32 addr) {
    void *state = ramPtr(addr, 8, "filter state");
    const s16 *h1 = sTable;
    s16 *h2 = sTable + 8;
    s16 h2Before[8], l1, l2, last[4];
    u32 count = align(sCount, 16), in = sIn, out = sOut, i;

    if (state == NULL) {
        return;
    }
    if (flags & A_INIT) {
        l1 = l2 = 0;
    } else {
        memcpy(last, state, sizeof(last));
        l1 = last[2];
        l2 = last[3];
    }
    for (i = 0; i < 8; i++) {
        h2Before[i] = h2[i];
        h2[i] = (s16)(((s32)h2[i] * gain) >> 14);
    }
    do {
        s16 frame[8];

        for (i = 0; i < 8; i++, in += 2) {
            frame[i] = *dmem16(in);
        }
        for (i = 0; i < 8; i++) {
            s32 accu = frame[i] * gain;
            accu += h1[i] * l1 + h2Before[i] * l2 + rdot(i, h2, frame);
            *dmem16(out + 2 * i) = clamp16(accu >> 14);
        }
        l1 = *dmem16(out + 12);
        l2 = *dmem16(out + 14);
        out += 16;
        count -= 16;
    } while (count != 0);
    for (i = 0; i < 4; i++) {
        last[i] = *dmem16(out - 8 + 2 * i);
    }
    storeState(state, last, sizeof(last));
}

static void runCommand(u32 w0, u32 w1) {
    u32 op = (w0 >> 24) & 0x7F, flags = (w0 >> 16) & 0xFF;

    switch (op) {
        case A_SPNOOP:
        case A_SEGMENT: /* only ever aSegment(0, 0): addresses are absolute */
            break;
        case A_CLEARBUFF: {
            u32 dmem = w0 & 0xFFFF, count = w1 & 0xFFF, i;
            if (count != 0) {
                count = align(count, 16);
                for (i = 0; i < count; i++) {
                    *dmem8(dmem + i) = 0;
                }
            }
            break;
        }
        case A_ADPCM:
            cmdAdpcm(flags, w1);
            break;
        case A_ENVMIXER:
            cmdEnvMixer(flags, w1);
            break;
        case A_LOADBUFF: {
            const void *src = ramPtr(w1 & ~7u, align(sCount, 8), "sample data");
            if (sCount != 0 && src != NULL) {
                loadBuffer(sIn & ~3u, src, align(sCount, 8));
            }
            break;
        }
        case A_RESAMPLE:
            cmdResample(flags, w0 & 0xFFFF, w1);
            break;
        case A_SAVEBUFF: {
            void *dst = ramPtr(w1 & ~7u, align(sCount, 8), "output buffer");
            if (sCount != 0 && dst != NULL) {
                saveBuffer(dst, sOut & ~3u, align(sCount, 8));
            }
            break;
        }
        case A_SETBUFF:
            if (flags & A_AUX) {
                sDryR = (u16)w0;
                sWetL = (u16)(w1 >> 16);
                sWetR = (u16)w1;
            } else {
                sIn = (u16)w0;
                sOut = (u16)(w1 >> 16);
                sCount = (u16)w1;
            }
            break;
        case A_SETVOL:
            if (flags & A_AUX) {
                sDry = (s16)w0;
                sWet = (s16)w1;
            } else {
                u32 lr = (flags & A_LEFT) ? 0 : 1;
                if (flags & A_VOL) {
                    sVol[lr] = (s16)w0;
                } else {
                    sTarget[lr] = (s16)w0;
                    sRate2[lr] = (s32)w1;
                }
            }
            break;
        case A_DMEMMOVE: {
            u32 dmemi = w0 & 0xFFFF, dmemo = w1 >> 16, count = w1 & 0xFFFF, i;
            if (count != 0) {
                count = align(count, 16);
                for (i = 0; i < count; i++) { /* forwards, byte by byte, like the RSP (regions may overlap) */
                    *dmem8(dmemo + i) = *dmem8(dmemi + i);
                }
            }
            break;
        }
        case A_LOADADPCM: {
            u32 count = align(w0 & 0xFFFF, 8) >> 1;
            const void *src;
            count = SDL_min(count, sizeof(sTable) / 2);
            src = ramPtr(w1, count * 2, "ADPCM book");
            if (src != NULL) {
                loadSamples(sTable, src, count);
            }
            break;
        }
        case A_MIXER:
            if (sCount != 0) {
                cmdMixer((s16)w0, w1 >> 16, w1 & 0xFFFF);
            }
            break;
        case A_INTERLEAVE:
            if (sCount != 0) {
                cmdInterleave(w1 >> 16, w1 & 0xFFFF);
            }
            break;
        case A_POLEF:
            if (sCount != 0) {
                cmdPoleFilter(flags, (u16)w0, w1);
            }
            break;
        case A_SETLOOP:
            sLoop = ramPtr(w1, 32, "ADPCM loop state");
            break;
        default:
            Port_Log("audio: unknown command %u", (unsigned)op);
            break;
    }
}

void Port_AudioRunTask(OSTask *task) {
    const u8 *list = (const u8 *)task->t.data_ptr;
    u32 n = task->t.data_size / 8, i;
    static int sTasks;

    if (ramPtr((u32)list, task->t.data_size, "command list") == NULL) {
        return;
    }
    for (i = 0; i < n; i++) {
        runCommand(Port_RdN64(list + 8 * i, 4), Port_RdN64(list + 8 * i + 4, 4));
    }
    if (sTasks++ == 0) {
        Port_Log("audio: first task, %u commands", (unsigned)n);
    }
}
