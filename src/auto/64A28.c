#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    s32 start;
    s32 end;
} Res;

Res *Res_FindType(s32);

s32 func_80063E28(s32 id) {
    s32 end = Res_FindType(id)->end;
    s32 start = Res_FindType(id)->start;

    if (id != 0) {
        return end - start + id;
    }
    return 0;
}
