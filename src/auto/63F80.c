#include "common.h"
#include "m2c.h"

extern s16 D_8011EEE8;
extern s8 D_8011EEFC;

void func_80063380(s16 arg0) {
    D_8011EEE8 = arg0;
    D_8011EEFC = 1;
}
