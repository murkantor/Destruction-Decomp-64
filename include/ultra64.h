#ifndef ULTRA64_H
#define ULTRA64_H

/* Minimal libultra declarations used by the decompiled code. */

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed long s32;
typedef unsigned long u32;
typedef volatile unsigned long vu32;
typedef signed long long s64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;

#define NULL 0
#define TRUE 1
#define FALSE 0

/*
 * PC port: the host is little-endian, and data is converted to host order when
 * it is loaded. Code that reads one byte or halfword of a word the rest of the
 * game treats as a whole u32 must find it at BE_OFF8(n) / BE_OFF16(n), and
 * bitfields sharing a word are declared in the reverse order under HOST_LE.
 */
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define HOST_LE 1
#define BE_OFF8(n) ((n) ^ 3)
#define BE_OFF16(n) ((n) ^ 2)
#else
#define BE_OFF8(n) (n)
#define BE_OFF16(n) (n)
#endif

/*
 * A double written as its two 32-bit words, high word first in the source:
 * DU_WORDS(hi, lo) initialises a struct of two u32 laid over an f64, in the
 * order the host keeps the words (low word first on a little-endian host).
 */
#ifdef HOST_LE
#define DU_WORDS(hi, lo) { lo, hi }
#else
#define DU_WORDS(hi, lo) { hi, lo }
#endif

/*
 * ASM_REG pins a local to a MIPS register, which some functions need to match.
 * ASM_ALIAS gives a declaration another symbol's name. On PC the pin is dropped
 * and the alias gets the platform's symbol prefix (_ on 32-bit Windows).
 */
/*
 * The RSP address of a KSEG0 pointer. On PC the renderer takes host pointers,
 * so the pointer is passed unchanged.
 */
#ifdef TARGET_PC
#define PHYS_ADDR(p) ((u32)(p))
#else
#define PHYS_ADDR(p) ((u32)(p) - 0x80000000)
#endif

/*
 * A buffer the game loads ROM data into must be in the port's RAM image, where
 * loaded data is tracked and converted to host order as it is read; a stack
 * buffer is not, and fields read straight from a local are never converted.
 * On PC such functions use a fixed slice of a scratch area at the top of RAM
 * instead (port/src/ram.c): PORT_DMA_BUF(type, name, offset) declares
 * `type *name` there; offsets are per call site, so each slice has one user.
 */
#ifdef TARGET_PC
/* aligned: otherwise GCC checks every field access as possibly unaligned, as a
   bulk access (__asan_loadN), which never converts */
extern u8 gPortScratch[] __attribute__((aligned(16)));
#define PORT_DMA_BUF(type, name, offset) type *name = (type *)(gPortScratch + (offset))
#endif

/*
 * Data the game unpacks itself (decompressed files) is big-endian like the
 * ROM it came from, but written a byte at a time; PORT_MARK_BE marks it for
 * conversion as it is read, as DMA does for data straight from the ROM.
 */
#ifdef TARGET_PC
void Port_MarkBE(const void *p, unsigned int len);
#define PORT_MARK_BE(p, len) Port_MarkBE((const void *)(p), (len))
#else
#define PORT_MARK_BE(p, len) ((void)0)
#endif

/* Loaded data the game reads only as bytes, in its own byte order (zip headers,
   little-endian by format): PORT_MARK_RAW keeps it as it is, even where GCC
   merges the byte reads into one wider load */
#ifdef TARGET_PC
void Port_MarkNative(const void *p, unsigned int len);
#define PORT_MARK_RAW(p, len) Port_MarkNative((const void *)(p), (len))
#else
#define PORT_MARK_RAW(p, len) ((void)0)
#endif

/*
 * Loaded data the game copies into a local or passes by value before reading
 * it (`f(obj, sp->pos)`): GCC does not instrument that copy, so it would stay
 * big-endian. PORT_CONVERT(p, count, width) converts count values of width
 * bytes at p first. The port logs such copies ("copy of big-endian data into
 * a local or argument").
 */
#ifdef TARGET_PC
void Port_Convert(void *p, unsigned int count, unsigned int width);
#define PORT_CONVERT(p, count, width) Port_Convert((void *)(p), (count), (width))
#else
#define PORT_CONVERT(p, count, width) ((void)0)
#endif

#ifdef TARGET_PC
#define ASM_STR2(x) #x
#define ASM_STR(x) ASM_STR2(x)
#define ASM_REG(r)
#define ASM_ALIAS(name) __asm__(ASM_STR(__USER_LABEL_PREFIX__) name)
#else
#define ASM_REG(r) asm(r)
#define ASM_ALIAS(name) __asm__(name)
#endif

typedef s32 OSPri;
typedef s32 OSId;

typedef struct {
    /* 0x00 */ u64 at, v0, v1, a0, a1, a2, a3;
    /* 0x38 */ u64 t0, t1, t2, t3, t4, t5, t6, t7;
    /* 0x78 */ u64 s0, s1, s2, s3, s4, s5, s6, s7;
    /* 0xB8 */ u64 t8, t9, gp, sp, s8, ra;
    /* 0xE8 */ u64 lo, hi;
    /* 0xF8 */ u32 sr, pc, cause, badvaddr, rcp;
    /* 0x10C */ u32 fpcsr;
    /* 0x110 */ f64 fp[16];
} __OSThreadContext; /* size = 0x190 */

typedef struct OSThread_s {
    /* 0x000 */ struct OSThread_s *next;
    /* 0x004 */ OSPri priority;
    /* 0x008 */ struct OSThread_s **queue;
    /* 0x00C */ struct OSThread_s *tlnext;
    /* 0x010 */ u16 state;
    /* 0x012 */ u16 flags;
    /* 0x014 */ OSId id;
    /* 0x018 */ s32 fp;
    /* 0x01C */ void *thprof;
    /* 0x020 */ __OSThreadContext context;
} OSThread; /* size = 0x1B0 */

typedef void *OSMesg;

typedef struct OSMesgQueue_s {
    /* 0x00 */ OSThread *mtqueue;
    /* 0x04 */ OSThread *fullqueue;
    /* 0x08 */ s32 validCount;
    /* 0x0C */ s32 first;
    /* 0x10 */ s32 msgCount;
    /* 0x14 */ OSMesg *msg;
} OSMesgQueue; /* size = 0x18 */

#define OS_MESG_NOBLOCK 0
#define OS_MESG_BLOCK 1

#define OS_STATE_STOPPED 1
#define OS_STATE_RUNNABLE 2
#define OS_STATE_RUNNING 4
#define OS_STATE_WAITING 8

#define OS_READ 0
#define OS_WRITE 1

#define OS_PRIORITY_IDLE 0

#define OS_TV_PAL 0
#define OS_TV_NTSC 1
#define OS_TV_MPAL 2

#define PI_STATUS_DMA_BUSY (1 << 0)
#define PI_STATUS_IO_BUSY (1 << 1)
#define PI_STATUS_ERROR (1 << 2)

extern s32 osTvType;

void osInitialize(void);
void osCreateThread(OSThread *t, OSId id, void (*entry)(void *), void *arg, void *sp, OSPri pri);
void osStartThread(OSThread *t);
void osSetThreadPri(OSThread *t, OSPri pri);
u32 osPiGetStatus(void);
s32 osPiRawStartDma(s32 direction, u32 devAddr, void *dramAddr, u32 size);

#endif
