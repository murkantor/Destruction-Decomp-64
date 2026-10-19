#include "common.h"
#include "m2c.h"

M2C_UNK Heap_Free(M2C_UNK *, M2C_UNK *);
M2C_UNK func_8006D190(M2C_UNK *, M2C_UNK *);
extern M2C_UNK D_800BCEF4;
extern M2C_UNK D_800BCEFC;
extern M2C_UNK D_800BCF04;
extern M2C_UNK D_800BCF0C;
extern volatile char D_800BCF98;
extern int D_80130250;
extern M2C_UNK gMainHeap;

void func_80056310(void)
{
  M2C_UNK *new_var;
  Heap_Free(&gMainHeap, &D_800BCF98);
  Heap_Free(&gMainHeap, (&D_800BCF98) + 8);
  new_var = &gMainHeap;
  func_8006D190(&D_80130250, &D_800BCF0C);
  Heap_Free(&gMainHeap, &D_800BCEF4);
  Heap_Free(new_var, &D_800BCEFC);
  Heap_Free(&gMainHeap, &D_800BCF04);
}
