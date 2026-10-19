#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x14];
    s32 dramout;
} ALSave;

s32 alSaveParam(void *filter, s32 paramID, void *param) {
    ALSave *a = filter;

    switch (paramID) {
        case 1:
            *(void **)filter = param;
            break;
        case 6:
            a->dramout = (s32)param;
            break;
        default:
            break;
    }
    return 0;
}
