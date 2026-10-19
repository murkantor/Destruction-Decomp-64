#include "common.h"
#include "m2c.h"

void alLink(void *arg0, void **arg1)
{
  void *temp_v0;
  temp_v0 = (void *) (*arg1);
  *((void ***) (((s8 *) arg0) + 4)) = arg1;
  *((void **) (((s8 *) arg0) + 0)) = temp_v0;
  temp_v0 = *arg1;
  if (temp_v0 != 0)
  {
    *((void **) (((s8 *) temp_v0) + 4)) = arg0;
  }
  *arg1 = arg0;
}
