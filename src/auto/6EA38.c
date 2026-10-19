#include "common.h"
#include "m2c.h"

typedef struct { u32 ramarray[15]; u32 pifstatus; } OSPifRam;
typedef struct { u8 dummy; u8 txsize; u8 rxsize; u8 cmd; u16 button; s8 stick_x; s8 stick_y; } __OSContReadFormat;
typedef struct { u16 button; s8 stick_x; s8 stick_y; u8 errnum; } OSContPad;
extern OSPifRam __osContPifRam;
extern u8 __osMaxControllers;

void osContGetReadData(OSContPad *data) {
    u8 *ptr;
    __OSContReadFormat readformat;
    int i;

    ptr = (u8 *)__osContPifRam.ramarray;
    for (i = 0; i < __osMaxControllers; i++, ptr += sizeof(__OSContReadFormat), data++) {
        readformat = *(__OSContReadFormat *)ptr;
        data->errnum = (readformat.rxsize & 0xC0) >> 4;
        if (data->errnum == 0) {
            data->button = readformat.button;
            data->stick_x = readformat.stick_x;
            data->stick_y = readformat.stick_y;
        }
    }
}
