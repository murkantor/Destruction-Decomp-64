#include "common.h"
#include "m2c.h"

M2C_UNK func_80058A74();                            /* extern */

void func_80058A40(void) {
    func_80058A74();
}

extern s32 D_8011EF04;

void func_80058A5C(s32 arg0) {
    D_8011EF04 = arg0;
}


void func_80058A68(void) {
    D_8011EF04 = 0;
}
