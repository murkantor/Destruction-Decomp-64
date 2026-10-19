#include "common.h"
#include "m2c.h"

typedef struct ALParam {
    struct ALParam *next;
} ALParam;
extern u8 *alGlobals;

void __freeParam(ALParam *param) {
    param->next = *(ALParam **)(alGlobals + 0x2C);
    *(ALParam **)(alGlobals + 0x2C) = param;
}
