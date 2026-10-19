#include "common.h"
#include "m2c.h"

typedef struct {
    u16 state;
    u16 retraceCount;
    void *framep;
    void *modep;
    u32 control;
    OSMesgQueue *msgq;
    OSMesg msg;
} __OSViContext;
typedef struct {
    u16 type;
    u8 pri;
    u8 status;
    OSMesgQueue *retQueue;
} OSIoMesgHdr;
typedef struct {
    OSIoMesgHdr hdr;
} OSIoMesg;
typedef struct {
    s32 active;
    OSThread *thread;
    OSMesgQueue *cmdQueue;
    OSMesgQueue *evtQueue;
} OSDevMgr;
extern u16 retrace;
extern u32 __osViIntrCount;
extern u64 __osCurrentTime;
extern u32 __osBaseCounter;
__OSViContext *__osViGetCurrentContext(void);
s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);
s32 osSendMesg(OSMesgQueue *, OSMesg, s32);
void __osViSwapContext(void);
void __osTimerInterrupt(void);
u32 osGetCount(void);

void viMgrMain(void *arg) {
    __OSViContext *vc;
    OSDevMgr *dm;
    OSIoMesg *mb;
    s32 first;
    u32 count;

    mb = NULL;
    first = 0;
    vc = __osViGetCurrentContext();
    retrace = vc->retraceCount;
    if (retrace == 0) {
        retrace = 1;
    }
    dm = (OSDevMgr *)arg;
    while (TRUE) {
        osRecvMesg(dm->evtQueue, (OSMesg *)&mb, OS_MESG_BLOCK);
        switch (mb->hdr.type) {
            case 13:
                __osViSwapContext();
                retrace--;
                if (retrace == 0) {
                    vc = __osViGetCurrentContext();
                    if (vc->msgq != NULL) {
                        osSendMesg(vc->msgq, vc->msg, OS_MESG_NOBLOCK);
                    }
                    retrace = vc->retraceCount;
                }
                __osViIntrCount++;
                if (first) {
                    count = osGetCount();
                    __osCurrentTime = count;
                    first = 0;
                }
                count = __osBaseCounter;
                __osBaseCounter = osGetCount();
                count = __osBaseCounter - count;
                __osCurrentTime = __osCurrentTime + count;
                break;
            case 14:
                __osTimerInterrupt();
                break;
        }
    }
}
