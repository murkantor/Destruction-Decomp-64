#include "common.h"
#include "m2c.h"

typedef struct {
    u16 type;
    u8 pri;
    u8 status;
    OSMesgQueue *retQueue;
} OSIoMesgHdr;
typedef struct {
    OSIoMesgHdr hdr;
    void *dramAddr;
    u32 devAddr;
    u32 size;
    void *piHandle;
} OSIoMesg;
typedef struct {
    s32 active;
} OSDevMgr;
extern OSDevMgr __osPiDevMgr;
OSMesgQueue *osPiGetCmdQueue(void);
s32 osJamMesg(OSMesgQueue *, OSMesg, s32);
s32 osSendMesg(OSMesgQueue *, OSMesg, s32);

s32 osPiStartDma(OSIoMesg *mb, s32 priority, s32 direction, u32 devAddr, void *dramAddr, u32 size, OSMesgQueue *mq) {
    register s32 ret;

    if (!__osPiDevMgr.active) {
        return -1;
    }
    if (direction == 0) {
        mb->hdr.type = 11;
    } else {
        mb->hdr.type = 12;
    }
    mb->hdr.pri = priority;
    mb->hdr.retQueue = mq;
    mb->dramAddr = dramAddr;
    mb->devAddr = devAddr;
    mb->size = size;
    mb->piHandle = NULL;
    if (priority == 1) {
        ret = osJamMesg(osPiGetCmdQueue(), (OSMesg)mb, OS_MESG_NOBLOCK);
    } else {
        ret = osSendMesg(osPiGetCmdQueue(), (OSMesg)mb, OS_MESG_NOBLOCK);
    }
    return ret;
}
