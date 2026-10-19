#include "common.h"
#include "m2c.h"

typedef struct {
    s16 type;
    s8 misc[30];
} OSScMsg;
typedef struct OSScTask_s {
    struct OSScTask_s *next;
    u32 state;
    u32 flags;
    void *framebuffer;
    union {
        struct {
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
        } t;
        long long force_structure_alignment;
    } list;
    OSMesgQueue *msgQ;
    OSMesg msg;
} OSScTask;
typedef struct OSScClient_s {
    struct OSScClient_s *next;
    OSMesgQueue *msgQ;
} OSScClient;
typedef struct {
    OSScMsg retraceMsg;
    OSScMsg prenmiMsg;
    OSMesgQueue interruptQ;
    OSMesg intBuf[8];
    OSMesgQueue cmdQ;
    OSMesg cmdMsgBuf[8];
    OSThread thread;
    OSScClient *clientList;
    OSScTask *audioListHead;
    OSScTask *gfxListHead;
    OSScTask *audioListTail;
    OSScTask *gfxListTail;
    OSScTask *curRSPTask;
    OSScTask *curRDPTask;
    u32 frameCount;
    s32 doAudio;
} OSSched;
extern char D_800BC208[];
extern char D_800BC20C[];
void __assert(const char *, const char *, int);
#define assert(EX, line) ((EX) ? ((void)0) : __assert(D_800BC208, D_800BC20C, line))
void __scHandleRetrace(OSSched *sc);
void __scHandleRSP(OSSched *sc);
void __scHandleRDP(OSSched *sc);
void __scAppendList(OSSched *sc, OSScTask *t);
s32 __scTaskComplete(OSSched *sc, OSScTask *t);
void __scExec(OSSched *sc, OSScTask *sp, OSScTask *dp);
void __scYield(OSSched *sc);
s32 __scSchedule(OSSched *sc, OSScTask **sp, OSScTask **dp, s32 availRCP);
s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);
s32 osSendMesg(OSMesgQueue *, OSMesg, s32);
u32 osSetIntMask(u32);

void osScRemoveClient(OSSched *sc, OSScClient *c) {
    OSScClient *client = sc->clientList;
    OSScClient *prev = 0;
    u32 mask;

    mask = osSetIntMask(1);
    while (client != 0) {
        if (client == c) {
            if (prev) {
                prev->next = c->next;
            } else {
                sc->clientList = c->next;
            }
            break;
        }
        prev = client;
        client = client->next;
    }
    osSetIntMask(mask);
}
