#include "common.h"
#include "m2c.h"

void alUnlink(void *arg0) {
    void **temp_v1_2;
    void *temp_v1;

    temp_v1 = M2C_FIELD(arg0, void **, 0);
    if (temp_v1 != NULL) {
        M2C_FIELD(temp_v1, void ***, 4) = (void **) M2C_FIELD(arg0, void ***, 4);
    }
    temp_v1_2 = M2C_FIELD(arg0, void ***, 4);
    if (temp_v1_2 != NULL) {
        *temp_v1_2 = M2C_FIELD(arg0, void **, 0);
    }
}
