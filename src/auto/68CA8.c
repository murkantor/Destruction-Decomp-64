#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Acmd;
typedef struct {
    u32 type;
    u32 flags;
    u64 *ucode_boot;
    u32 ucode_boot_size;
    u64 *ucode;
    u32 ucode_size;
    u64 *ucode_data;
    u32 ucode_data_size;
    u64 *dram_stack;
    u32 dram_stack_size;
    u64 *output_buff;
    u64 *output_buff_size;
    u64 *data_ptr;
    u32 data_size;
    u64 *yield_data_ptr;
    u32 yield_data_size;
} OSTask_t;
typedef struct OSScTask_s {
    struct OSScTask_s *next;
    u32 state;
    u32 flags;
    void *framebuffer;
    OSTask_t list;
    OSMesgQueue *msgQ;
    OSMesg msg;
} OSScTask;
typedef struct {
    s16 *data;
    s16 frameSamples;
    OSScTask task;
    u8 pad[0x10];
    u8 msg[1];
} AudioInfo;
typedef struct {
    Acmd *ACMDList[2];
    u8 pad[0x1F8];
    OSMesgQueue audioReplyMsgQ;
} AMStruct;
extern AMStruct D_8011F080;
extern s32 D_800B4E48;
extern u32 D_800CEAE0;
extern u32 D_800CEADC;
extern void *D_800CEB08;
extern u8 D_8007ADD0[];
extern u8 D_8007AEA0[];
extern u8 rspbootTextEnd[] asm("D_8007AEA0");
extern u8 D_800B72A0[];
void Audio_RecycleVoices(void);
u32 osVirtualToPhysical(void *);
s32 osAiSetNextBuffer(void *, u32);
u32 osAiGetLength(void);
Acmd *alAudioFrame(Acmd *, s32 *, s16 *, s32);
OSMesgQueue *osScGetCmdQ(void *);
s32 osSendMesg(OSMesgQueue *, OSMesg, s32);

u32 func_800680A8(AudioInfo *info, AudioInfo *lastInfo) {
    s16 *audioPtr;
    Acmd *cmdp;
    s32 cmdLen;
    int samplesLeft = 0;
    OSScTask *t;

    Audio_RecycleVoices();

    audioPtr = (s16 *)osVirtualToPhysical(info->data);

    if (lastInfo) {
        osAiSetNextBuffer(lastInfo->data, lastInfo->frameSamples << 2);
    }

    samplesLeft = osAiGetLength() >> 2;
    info->frameSamples = (16 + (D_800CEAE0 - samplesLeft + 80)) & ~0xf;
    if (info->frameSamples < D_800CEADC) {
        info->frameSamples = D_800CEADC;
    }

    cmdp = alAudioFrame(D_8011F080.ACMDList[D_800B4E48], &cmdLen, audioPtr, info->frameSamples);

    if (cmdLen == 0) {
        return 0;
    }

    t = &info->task;
    t->next = 0;
    t->msgQ = &D_8011F080.audioReplyMsgQ;
    t->msg = (OSMesg)&info->msg;
    t->flags = 2;
    t->list.data_ptr = (u64 *)D_8011F080.ACMDList[D_800B4E48];
    t->list.data_size = (cmdp - D_8011F080.ACMDList[D_800B4E48]) * sizeof(Acmd);
    t->list.type = 2;
    t->list.ucode_boot = (u64 *)D_8007ADD0;
    t->list.ucode_boot_size = ((int)rspbootTextEnd - (int)D_8007ADD0);
    t->list.flags = 0;
    t->list.ucode = (u64 *)D_8007AEA0;
    t->list.ucode_data = (u64 *)D_800B72A0;
    t->list.ucode_data_size = 0x800;
    t->list.dram_stack = (u64 *)NULL;
    t->list.dram_stack_size = 0;
    t->list.output_buff = (u64 *)NULL;
    t->list.output_buff_size = 0;
    t->list.yield_data_ptr = NULL;
    t->list.yield_data_size = 0;

    osSendMesg(osScGetCmdQ(D_800CEB08), (OSMesg)t, 1);

    D_800B4E48 ^= 1;

    return 1;
}
