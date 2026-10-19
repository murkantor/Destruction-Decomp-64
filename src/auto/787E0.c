#include "common.h"
#include "m2c.h"

typedef struct { u8 align; u8 txsize; u8 rxsize; u8 poll; u8 typeh; u8 typel; u8 status; u8 align1; } OSContPackedRequest;
typedef struct { u16 type; u8 status; u8 errno; } OSContStatus;
extern u8 __osMaxControllers;
extern u8 __osPfsPifRam[];
void __osPfsGetInitData(u8 *pattern, OSContStatus *data) {
    u8 *ptr;
    OSContPackedRequest requestHeader;
    s32 i;
    u8 bits = 0;

    ptr = __osPfsPifRam;
    for (i = 0; i < __osMaxControllers; i++, ptr += sizeof(OSContPackedRequest), data++) {
        requestHeader = *(OSContPackedRequest *)ptr;
        data->errno = (requestHeader.rxsize & 0xC0) >> 4;
        if (data->errno == 0) {
            data->type = (requestHeader.typel << 8) | requestHeader.typeh;
            data->status = requestHeader.status;
            bits |= 1 << i;
        }
    }
    *pattern = bits;
}
