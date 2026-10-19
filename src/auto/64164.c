#include "common.h"
#include "m2c.h"

extern s16 D_8011EEEA;
extern s8 D_8011EEEC;

void func_80063564(s16 arg0, s8 arg1) {
    D_8011EEEA = arg0;
    D_8011EEEC = arg1;
}
