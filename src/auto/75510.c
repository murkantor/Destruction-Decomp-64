#include "common.h"
#include "m2c.h"

typedef struct {
    u32 ctrl;
    u32 pad[0x4F / 4];
} OSViMode;
typedef struct {
    u16 state;
    u16 retraceCount;
    void *framep;
    u32 *modep;
    u32 control;
    u8 pad10[0x20];
} __OSViContext;
extern __OSViContext vi[2];
extern __OSViContext *__osViCurr;
extern __OSViContext *__osViNext;
extern s32 osTvType;
extern u32 osViModeNtscLan1[];
extern u32 osViModePalLan1[];
extern u32 osViModeMpalLan1[];
void bzero(void *, s32);
void __osViSwapContext(void);

void __osViInit(void) {
    bzero(vi, sizeof(vi));
    __osViCurr = &vi[0];
    __osViNext = &vi[1];
    vi[1].retraceCount = 1;
    vi[0].retraceCount = 1;
    vi[1].framep = (void *)0x80000000;
    vi[0].framep = (void *)0x80000000;
    if (osTvType == 0) {
        vi[1].modep = osViModePalLan1;
    } else if (osTvType == 2) {
        vi[1].modep = osViModeMpalLan1;
    } else {
        vi[1].modep = osViModeNtscLan1;
    }
    __osViNext->state = 0x20;
    __osViNext->control = __osViNext->modep[1];
    while (*(vu32 *)0xA4400010 > 10) {
    }
    *(vu32 *)0xA4400000 = 0;
    __osViSwapContext();
}
