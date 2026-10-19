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

s32 osEPiStartDma(void *pihandle, OSIoMesg *mb, s32 direction) {
    register s32 ret;

    if (!__osPiDevMgr.active) {
        return -1;
    }
    mb->piHandle = pihandle;
    if (direction == 0) {
        mb->hdr.type = 15;
    } else {
        mb->hdr.type = 16;
    }
    if (mb->hdr.pri == 1) {
        ret = osJamMesg(osPiGetCmdQueue(), (OSMesg)mb, OS_MESG_NOBLOCK);
    } else {
        ret = osSendMesg(osPiGetCmdQueue(), (OSMesg)mb, OS_MESG_NOBLOCK);
    }
    return ret;
}
