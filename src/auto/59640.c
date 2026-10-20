#include "common.h"
#include "m2c.h"

M2C_UNK Cont_ReadAll();                            /* extern */

void func_80058A40(void) {
    Cont_ReadAll();
}

extern s32 D_8011EF04;

void Pad_SetReadHook(s32 arg0) {
    D_8011EF04 = arg0;
}


void Pad_ClearReadHook(void) {
    D_8011EF04 = 0;
}
