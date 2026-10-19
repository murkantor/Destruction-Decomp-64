#include "common.h"
#include "m2c.h"

typedef struct { u8 dummy; u8 txsize; u8 rxsize; u8 cmd; u8 typeh; u8 typel; u8 status; u8 dummy1; } __OSContRequesFormat;
typedef struct { u16 type; u8 status; u8 errno; } OSContStatus;
extern u8 __osMaxControllers;
extern u8 __osContPifRam[];

void __osContGetInitData(u8 *pattern, OSContStatus *data) {
    u8 *ptr;
    __OSContRequesFormat requestHeader;
    s32 i;
    u8 bits;

    bits = 0;
    ptr = (u8 *)__osContPifRam;
    for (i = 0; i < __osMaxControllers; i++, ptr += sizeof(requestHeader), data++) {
        requestHeader = *(__OSContRequesFormat *)ptr;
        data->errno = (requestHeader.rxsize & 0xC0) >> 4;
        if (data->errno == 0) {
            data->type = requestHeader.typel << 8 | requestHeader.typeh;
            data->status = requestHeader.status;
            bits |= 1 << i;
        }
    }
    *pattern = bits;
}
