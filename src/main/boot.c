#include "common.h"

typedef struct {
    /* 0x0 */ void *heapStart;
    /* 0x4 */ s32 unk4;
    /* 0x8 */ s32 videoMode; /* 0 = NTSC, 1 = PAL, 2 = MPAL */
} GameInitConfig;

/* Code already copied by IPL3: 1 MB minus the 0x1000 byte header area. */
#define IPL3_LOAD_SIZE 0xFFC00

extern u8 _mainSegmentStart[];
extern u8 _mainSegmentRomStart[];
extern u8 _mainSegmentRomEnd[];

extern OSThread sIdleThread;
extern OSThread sMainThread;
extern u8 sIdleStackTop[];
extern u8 sMainStackTop[];
extern GameInitConfig gInitConfig;
extern u8 gHeapStart[];
extern s32 D_800B4D80;

void idle(void *arg);
void mainproc(void *arg);
void func_80058800(void);
void Main_InitSystems(s32, GameInitConfig *);
void Boot_LoadData(void);

void boot(void) {
    u32 size = _mainSegmentRomEnd - _mainSegmentRomStart;

    /* Copy whatever IPL3 did not. This build is smaller than 1 MB, so it never runs. */
    if (size > IPL3_LOAD_SIZE) {
        s32 busy;

        osPiRawStartDma(OS_READ, (u32)(_mainSegmentRomStart + IPL3_LOAD_SIZE),
                        _mainSegmentStart + IPL3_LOAD_SIZE, size - IPL3_LOAD_SIZE);
        busy = TRUE;
        do {
            switch ((s32)osPiGetStatus()) {
                case PI_STATUS_DMA_BUSY:
                case PI_STATUS_IO_BUSY:
                case PI_STATUS_ERROR:
                    break;
                default:
                    busy = FALSE;
                    break;
            }
        } while (busy);
    }

    osInitialize();
    osCreateThread(&sIdleThread, 1, idle, NULL, sIdleStackTop, 10);
    osStartThread(&sIdleThread);
}

void idle(void *arg) {
    osCreateThread(&sMainThread, 4, mainproc, arg, sMainStackTop, 2);
    osStartThread(&sMainThread);
    osSetThreadPri(NULL, OS_PRIORITY_IDLE);
    for (;;) {
    }
}

void mainproc(void *arg) {
    gInitConfig.heapStart = gHeapStart;
    gInitConfig.unk4 = 1500;

    if (osTvType == OS_TV_PAL) {
        gInitConfig.videoMode = 1;
    }
    if (osTvType == OS_TV_NTSC) {
        gInitConfig.videoMode = 0;
    }
    if (osTvType == OS_TV_MPAL) {
        gInitConfig.videoMode = osTvType;
    }

    func_80058800();
    Main_InitSystems(120, &gInitConfig);
    Boot_LoadData();
}

s16 func_80064D70(void) {
    return D_800B4D80++;
}

s32 func_80064D90(u8 *arg0) {
    *(s32 *)(arg0 + 0x34) = 0;
    *(s32 *)(arg0 + 0x30) = 0;
    *(s32 *)(arg0 + 0x70) = 0;
    *(s16 *)(arg0 + 0x9E) = 0;
    *(s32 *)(arg0 + 0x40) = 0;
    return 0;
}
