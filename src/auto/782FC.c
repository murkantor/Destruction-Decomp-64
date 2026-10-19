#include "common.h"
#include "m2c.h"

typedef struct { u8 txsize; u8 rxsize; u8 cmd; u8 typeh; u8 typel; u8 status; } __OSContRequesFormatShort;
typedef struct { u16 type; u8 status; u8 errno; } OSContStatus;
extern u8 __osPfsPifRam[];
#define CHNL_ERR(format) (((format).rxsize & 0xC0) >> 4)

void __osPfsGetOneChannelData(s32 channel, OSContStatus *data) {
    u8 *ptr;
    __OSContRequesFormatShort requestformat;
    s32 i;

    ptr = __osPfsPifRam;
    for (i = 0; i < channel; i++) {
        ptr++;
    }
    requestformat = *((__OSContRequesFormatShort *)ptr);
    data->errno = CHNL_ERR(requestformat);
    if (data->errno == 0) {
        data->type = (requestformat.typel << 8) | requestformat.typeh;
        data->status = requestformat.status;
    }
}
